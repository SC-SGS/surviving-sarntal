//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Hiker.h"
#include "../../output/haptics/HapticsService.hpp"
#include "../../spawner/PolygonGenerator.h"
#include "spdlog/spdlog.h"

Hiker::Hiker(const Vector position,
             AudioService &audioService,
             InputHandler &inputHandler,
             HikerConstants hikerConstants)
    : RenderedEntity(position), audioService(audioService), inputHandler(inputHandler), hikerConstants(hikerConstants) {
    velocity = {0, 0};
    height = hikerConstants.hikerSize;
    width = hikerConstants.hikerWidth;
    healthPoints = hikerConstants.hikerMaxHealth;
    hikerMovement = HikerMovement();
    isAlive = true;
    animation = {4, 0, 0.3, 0};
    this->walkingHitboxDelta = {0, height / 2};
    Vector centerWalking = position + this->walkingHitboxDelta;
    floatType halfHeightWalking = height / 2;
    std::vector<Vector> verticesWalking = {
        {0, -halfHeightWalking}, {halfHeightWalking / 4, 0}, {0, halfHeightWalking}, {-halfHeightWalking / 4, 0}};
    floatType densityWalking = this->hikerConstants.mass / PolygonGenerator::calculateAreaBodySpace(verticesWalking);
    this->hitboxWalking = std::make_shared<DynamicConvexPolygon>(
        centerWalking, verticesWalking, std::vector<Vector2>{verticesWalking.size() + 1}, this->hikerConstants.mass,
        densityWalking, PolygonGenerator::calculateInertiaRotCentroidBodySpace(verticesWalking, densityWalking),
        DynamicProperties{});
    floatType halfHeightCrouched = (height * hikerConstants.hikerCrouchRatio) / 2;
    this->crouchedHitboxDelta = {0, halfHeightCrouched};
    Vector centerCrouched = position + this->crouchedHitboxDelta;
    std::vector<Vector> verticesCrouched = {
        {0, -halfHeightCrouched}, {halfHeightCrouched / 4, 0}, {0, halfHeightCrouched}, {-halfHeightCrouched / 4, 0}};
    floatType densityCrouched = this->hikerConstants.mass / PolygonGenerator::calculateAreaBodySpace(verticesCrouched);
    this->hitboxCrouched = std::make_shared<DynamicConvexPolygon>(
        centerCrouched, verticesCrouched, std::vector<Vector2>{verticesCrouched.size() + 1}, this->hikerConstants.mass,
        densityCrouched, PolygonGenerator::calculateInertiaRotCentroidBodySpace(verticesCrouched, densityCrouched),
        DynamicProperties{});
    this->crouchedToWalkingDelta = this->walkingHitboxDelta.y - this->crouchedHitboxDelta.y;
    spdlog::info("A Hiker was initialized");
}

RenderInformation Hiker::getRenderInformation() const {
    floatType renderWidth = this->width;
    floatType renderHeight = this->height;
    // TODO due to the shield, the hiker itself in the texture is smaller (and therefore rendered smaller bc
    // hikerheight/width stays the same), so I thought we could render it a little big bigger - but I know this is not a
    // pretty solution I was thinking about introducing a new hikerHeight/Width for hiker with shield
    if (this->hasShield()) {
        renderWidth += 0.1;
        renderHeight += 0.1;
    }
    const floatType directedWidth =
        renderWidth * static_cast<float>(hikerMovement.getDirection() != HikerMovement::LEFT ? 1 : -1);

    return RenderInformation{Vector2(position),
                             {0, height / 2},
                             directedWidth,
                             renderHeight,
                             0,
                             hikerMovement.getStateString(this->hasShield()),
                             animation};
}

floatType Hiker::getHeight() const { return height; }

void Hiker::setHeight(const floatType height) { this->height = height; }

floatType Hiker::getWidth() const { return width; }

void Hiker::setWidth(const floatType width) { this->width = width; }

int Hiker::getHealthPoints() const { return healthPoints; }

void Hiker::setHealthPoints(const int healthPoints) {
    if (healthPoints <= 0) {
        this->healthPoints = 0;
        this->isAlive = false;
    } else if (healthPoints >= hikerConstants.hikerMaxHealth) {
        this->healthPoints = hikerConstants.hikerMaxHealth;
    } else {
        this->healthPoints = healthPoints;
    }
}

void Hiker::addHealthPoints(const int points) {
    this->setHealthPoints(std::min(this->healthPoints + points, hikerConstants.hikerMaxHealth));
}

void Hiker::doDamagePoints(const int damagePoints) {
    if (!this->hasShield()) {
        this->setHealthPoints(std::max(this->healthPoints - damagePoints, 0));
    }
}

void Hiker::setShield(const double time) { this->shieldTime = GetTime() + time; }

bool Hiker::hasShield() const { return this->shieldTime > GetTime(); }

int Hiker::getShieldTimeLeft() const { return static_cast<int>(this->shieldTime - GetTime()); }

const HikerMovement &Hiker::getHikerMovement() const { return hikerMovement; }

void Hiker::setHikerMovement(const HikerMovement &movement) {
    switch (movement.getState()) {
    case HikerMovement::MOVING:
        height = hikerConstants.hikerSize;
        width = hikerConstants.hikerWidth;
        break;
    case HikerMovement::CROUCHED:
        height = hikerConstants.hikerSize * hikerConstants.hikerCrouchRatio;
        width = hikerConstants.hikerWidth * hikerConstants.hikerCrouchRatio;
        break;
    case HikerMovement::IN_AIR:
        height = hikerConstants.hikerSize;
        width = hikerConstants.hikerWidth;
        break;
    }
    hikerMovement = movement;
}

const Vector &Hiker::getVelocity() const { return velocity; }

void Hiker::setVelocity(const Vector &newVel) {
    this->setXVelocity(newVel.x);
    this->setYVelocity(newVel.y);
}

bool Hiker::getIsAlive() const { return this->isAlive; }

void Hiker::setIsAlive(const bool alive) { isAlive = alive; }

void Hiker::turnLeft() { this->hikerMovement.setDirection(HikerMovement::LEFT); }
void Hiker::turnRight() { this->hikerMovement.setDirection(HikerMovement::RIGHT); }
void Hiker::turnNeutral() { this->hikerMovement.setDirection(HikerMovement::NEUTRAL); }
void Hiker::crouch() {
    if (this->hikerMovement.getState() == HikerMovement::MOVING) {
        this->audioService.playSound("crouch");
        this->hikerMovement.setState(HikerMovement::CROUCHED);
        this->setHeight(hikerConstants.hikerSize * hikerConstants.hikerCrouchRatio);
        this->setWidth(hikerConstants.hikerWidth * hikerConstants.hikerCrouchRatio);
    }
}
void Hiker::uncrouch() {
    if (this->hikerMovement.getState() == HikerMovement::CROUCHED) {
        hikerMovement.setState(HikerMovement::MOVING);
        this->setHeight(hikerConstants.hikerSize);
        this->setWidth(hikerConstants.hikerWidth);
        this->shouldUncrouch = false;
    }
}
void Hiker::jump() {
    if (hikerMovement.getState() == HikerMovement::CROUCHED) {
        return;
    }
    if (hikerMovement.getState() != HikerMovement::IN_AIR) {
        hikerMovement.setLastJump(0.0);
        hikerMovement.setCanJumpAgain(true);
    }
    const bool canJump = hikerMovement.getLastJump() < 1.5 && hikerMovement.getCanJumpAgain();
    if (canJump) {
        this->audioService.interruptMovingSound();
        this->audioService.playSound("jump");
        this->velocity.setY(hikerConstants.jumpVelocity);
        this->setHitboxVelocity(this->getVelocity());
        if (this->hikerMovement.getState() == HikerMovement::IN_AIR) {
            this->hikerMovement.setCanJumpAgain(false);
        }
        hikerMovement.setState(HikerMovement::IN_AIR);
    }
}

void Hiker::setMoving() {
    if (this->hikerMovement.getState() == HikerMovement::IN_AIR) {
        this->move(Vector{0.f, 0.f});
    }
    if (this->hikerMovement.getState() != HikerMovement::CROUCHED) {
        this->hikerMovement.setState(HikerMovement::MOVING);
        this->setHeight(hikerConstants.hikerSize);
        this->setWidth(hikerConstants.hikerWidth);
    }
    this->setVelocity({0.f, 0.f});
    this->hikerMovement.setDirection(HikerMovement::NEUTRAL);
    this->setLastJump(0.0f);
}

void Hiker::setInAir() {
    if (this->hikerMovement.getState() == HikerMovement::CROUCHED) {
        this->uncrouch();
    }
    this->hikerMovement.setState(HikerMovement::IN_AIR);
    this->setLastJump(0.0);
    this->setHeight(hikerConstants.hikerSize);
    this->setWidth(hikerConstants.hikerWidth);
    this->audioService.interruptMovingSound();
}

void Hiker::moveToRight(const floatType deltaX) { this->position.setX(this->position.x + deltaX); }
void Hiker::moveToLeft(const floatType deltaX) { this->position.setX(this->position.x - deltaX); }
void Hiker::accelerateX(const floatType deltaX) {
    this->velocity.setX(this->velocity.x + deltaX);
    this->setHitboxVelocity(this->getVelocity());
    this->updateDirection();
}
void Hiker::accelerateY(const floatType deltaY) {
    this->velocity.setY(this->velocity.y + deltaY);
    this->setHitboxVelocity(this->getVelocity());
}
void Hiker::setXVelocity(const floatType xValue) {
    this->velocity.setX(xValue);
    this->setHitboxVelocity(this->getVelocity());
    this->updateDirection();
}

void Hiker::setYVelocity(const floatType yValue) {
    this->velocity.setY(yValue);
    this->setHitboxVelocity(this->getVelocity());
}
void Hiker::setLastJump(const float lastJump) { this->hikerMovement.setLastJump(lastJump); }

void Hiker::kill() {
    this->isAlive = false;
    this->healthPoints = 0;
    this->velocity = {0, 0};
    this->shieldTime = 0.0;
    this->setHitboxVelocity(this->getVelocity());
    HapticsService::deathRumble(this->inputHandler);
}

void Hiker::reset(const Vector &position) {
    this->setIsAlive(true);
    this->shieldTime = 0.0;
    this->setHealthPoints(this->hikerConstants.hikerMaxHealth);
    this->setPosition(position);
    this->setHikerMovement({HikerMovement::MOVING, HikerMovement::NEUTRAL});
    this->setMoving();
    this->setKnockback({0.f, 0.f});
    this->setVelocity({0.f, 0.f});
    this->setHitboxVelocity(this->getVelocity());
    this->resetHitboxAngularMomentum();
}

const Vector &Hiker::getCurrentHitboxDelta() const {
    if (this->hikerMovement.getState() == HikerMovement::CROUCHED) {
        return this->crouchedHitboxDelta;
    }
    return this->walkingHitboxDelta;
}

floatType Hiker::computeSpeedFactor(const Vector &movement) const {
    if (movement.x == 0) {
        return 1.0f;
    }
    const floatType slope = movement.computeSlope();
    floatType speedFactor;
    if (movement.y < 0) { // Downhill
        speedFactor = std::fmin(this->hikerConstants.maxSpeedFactorNegSlope,
                                1.0f + (slope / this->hikerConstants.maxClimbableSlope) *
                                           (this->hikerConstants.maxSpeedFactorNegSlope - 1.0f));
    } else { // Uphill
        speedFactor = std::fmax(
            0.0f, ((this->hikerConstants.maxClimbableSlope - slope) / this->hikerConstants.maxClimbableSlope));
    }
    return speedFactor;
}

std::shared_ptr<DynamicConvexPolygon> Hiker::getCurrentHitbox() const {
    if (this->hikerMovement.getState() == HikerMovement::CROUCHED) {
        return this->hitboxCrouched;
    }
    return this->hitboxWalking;
}

void Hiker::setPosition(const Vector &position) {
    RenderedEntity::setPosition(position);
    this->hitboxWalking->setPosition(position + this->walkingHitboxDelta);
    this->hitboxCrouched->setPosition(position + this->crouchedHitboxDelta);
}
void Hiker::move(const Vector &movement) {
    this->hitboxWalking->move(movement);
    this->hitboxCrouched->move(movement);
    this->position += movement;
    if (movement.length() < NUMERIC_EPSILON) {
        this->audioService.interruptMovingSound();
    }
}

const Vector &Hiker::getKnockback() const { return knockback; }
void Hiker::setKnockback(const Vector &newKnockback) { this->knockback = newKnockback; }

void Hiker::setHitboxVelocity(const Vector &velocity) {
    this->hitboxWalking->setLinearMomentum(velocity * this->hikerConstants.mass);
    this->hitboxCrouched->setLinearMomentum(velocity * this->hikerConstants.mass);
}
void Hiker::updateDirection() {
    if (this->velocity.x < 0) {
        this->hikerMovement.setDirection(HikerMovement::LEFT);
    } else if (this->velocity.x > 0) {
        this->hikerMovement.setDirection(HikerMovement::RIGHT);
    } else {
        this->hikerMovement.setDirection(HikerMovement::NEUTRAL);
    }
}

void Hiker::resetHitboxAngularMomentum() {
    this->hitboxWalking->setAngularMomentum(0.f);
    this->hitboxCrouched->setAngularMomentum(0.f);
}

void Hiker::setShouldUncrouch() { this->shouldUncrouch = true; }
bool Hiker::getShouldUncrouch() const { return this->shouldUncrouch; }
std::shared_ptr<DynamicConvexPolygon> Hiker::getHitboxWalking() const { return hitboxWalking; }
std::shared_ptr<DynamicConvexPolygon> Hiker::getHitboxCrouched() const { return hitboxCrouched; }
