//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Hiker.h"
#include "../../output/haptics/HapticsService.hpp"
#include "spdlog/spdlog.h"

Hiker::Hiker(const Vector position, AudioService &audioService, HikerConstants hikerConstants)
    : RenderedEntity(position), audioService(audioService), hikerConstants(hikerConstants) {

    velocity = {0, 0};
    height = hikerConstants.hikerHeight;
    width = hikerConstants.hikerWidth;
    healthPoints = hikerConstants.hikerMaxHealth;
    hikerMovement = HikerMovement();
    isAlive = true;
    animation = {4, 0, 0.3, 0};
    this->walkingHitBoxDelta = {0, hikerConstants.hikerHeight / 2.0f};
    Vector center = position + this->walkingHitBoxDelta;
    floatType halfHeight = this->hikerConstants.hikerHeight / 2;
    floatType halfWidth = this->hikerConstants.hikerWidth / 2;
    // TODO: Nice hitbox, currently this is just a diamond shape
    std::vector<Vector> vertices = {{0, -halfHeight}, {halfHeight / 4, 0}, {0, halfHeight}, {-halfHeight / 4, 0}};
    this->boundingBoxWalking = std::make_shared<DynamicConvexPolygon>(
        center, vertices, std::vector<Vector2>{vertices.size() + 1}, this->hikerConstants.mass, 0.0f,
        std::numeric_limits<floatType>::infinity(), DynamicProperties{});

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
    } else if (healthPoints >= 100) {
        this->healthPoints = 100;
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

const HikerMovement &Hiker::getHikerMovement() const { return hikerMovement; }

void Hiker::setHikerMovement(const HikerMovement &movement) {
    switch (movement.getState()) {
    case HikerMovement::MOVING:
        height = hikerConstants.hikerHeight;
        width = hikerConstants.hikerWidth;
        break;
    case HikerMovement::CROUCHED:
        height = hikerConstants.crouchedHikerHeight;
        width = hikerConstants.crouchedHikerWidth;
        break;
    case HikerMovement::IN_AIR:
        height = hikerConstants.hikerHeight;
        width = hikerConstants.hikerWidth;
        break;
    }
    hikerMovement = movement;
}

const std::vector<HitInformation> &Hiker::getHitInformation() const { return hitInformation; }

void Hiker::setHitInformation(const std::vector<HitInformation> &hits) { hitInformation = hits; }

bool Hiker::getIsHit() const { return isHit; }

void Hiker::setIsHit(const bool isHit) { this->isHit = isHit; }

const Vector &Hiker::getVelocity() const { return velocity; }

void Hiker::setVelocity(const Vector &newVel) { velocity = newVel; }

bool Hiker::getIsAlive() const { return this->isAlive; }

void Hiker::setIsAlive(const bool alive) { isAlive = alive; }

void Hiker::turnLeft() { this->hikerMovement.setDirection(HikerMovement::LEFT); }
void Hiker::turnRight() { this->hikerMovement.setDirection(HikerMovement::RIGHT); }
void Hiker::turnNeutral() { this->hikerMovement.setDirection(HikerMovement::NEUTRAL); }
void Hiker::crouch() {
    if (this->hikerMovement.getState() == HikerMovement::MOVING) {
        this->audioService.playSound("crouch");
        this->hikerMovement.setState(HikerMovement::CROUCHED);
        this->setHeight(hikerConstants.crouchedHikerHeight);
        this->setWidth(hikerConstants.crouchedHikerWidth);
    }
}
void Hiker::uncrouch() {
    if (this->hikerMovement.getState() == HikerMovement::CROUCHED) {
        hikerMovement.setState(HikerMovement::MOVING);
        this->setHeight(hikerConstants.hikerHeight);
        this->setWidth(hikerConstants.hikerWidth);
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
    const bool canSecondJump = hikerMovement.getLastJump() < 1.5 && hikerMovement.getCanJumpAgain();
    if (canSecondJump) {
        doSecondJump();
    }
}
void Hiker::setHikerMoving() { this->hikerMovement.setState(HikerMovement::MOVING); }
void Hiker::setHikerInAir() { this->hikerMovement.setState(HikerMovement::IN_AIR); }
void Hiker::moveToRight(const floatType deltaX) { this->position.setX(this->position.x + deltaX); }
void Hiker::moveToLeft(const floatType deltaX) { this->position.setX(this->position.x - deltaX); }
void Hiker::accelerateX(const floatType deltaX) { this->velocity.setX(this->velocity.x + deltaX); }
void Hiker::accelerateY(const floatType deltaY) { this->velocity.setY(this->velocity.y + deltaY); }
void Hiker::setXVelocity(const floatType xValue) { this->velocity.setX(xValue); }
void Hiker::setYVelocity(const floatType yValue) { this->velocity.setY(yValue); }
void Hiker::setLastJump(const float lastJump) { this->hikerMovement.setLastJump(lastJump); }
void Hiker::doSecondJump() {
    this->audioService.playSound("jump");
    this->velocity.setY(hikerConstants.jumpVelocity);
    if (this->hikerMovement.getState() == HikerMovement::IN_AIR) {
        this->hikerMovement.setCanJumpAgain(false);
    }
    hikerMovement.setState(HikerMovement::IN_AIR);
}

void Hiker::kill() {
    this->isAlive = false;
    this->healthPoints = 0;
    this->velocity = {0, 0};
    HapticsService::deathRumble();
}

void Hiker::reset(const Vector &position) {
    this->setIsAlive(true);
    this->setHealthPoints(this->hikerConstants.hikerMaxHealth);
    this->setPosition(position);
}

AxisAlignedBoundingBox Hiker::getBoundingBoxMovement(const Vector movement) const {
    const AxisAlignedBoundingBox polyBoundingBox =
        AxisAlignedBoundingBox::transform(this->getCurrentBoundingBox()->getBoundingBox());
    const AxisAlignedBoundingBox startBox = polyBoundingBox;
    const AxisAlignedBoundingBox endBox = polyBoundingBox.moveByDelta(movement);
    return startBox.merge(endBox);
}

floatType Hiker::computeSpeedFactor(const Vector &movement) const {
    if (movement.x == 0) {
        return 0.0;
    }
    const floatType slope = movement.computeSlope();
    floatType speedFactor;
    if (movement.y < 0) { // Downhill
        speedFactor = std::fmin(this->hikerConstants.maxSpeedNegSlope,
                                this->hikerConstants.normalSpeed +
                                    (slope / this->hikerConstants.maxClimbableSlope) *
                                        (this->hikerConstants.maxSpeedNegSlope - this->hikerConstants.normalSpeed));
    } else { // Uphill
        speedFactor = std::fmax(
            0.0f, ((this->hikerConstants.maxClimbableSlope - slope) / this->hikerConstants.maxClimbableSlope) *
                      this->hikerConstants.normalSpeed);
    }
    return speedFactor;
}

std::shared_ptr<DynamicConvexPolygon> Hiker::getCurrentBoundingBox() const { return this->boundingBoxWalking; }

void Hiker::setPosition(const Vector &position) {
    RenderedEntity::setPosition(position);
    const Vector boundingBoxWalkingPosition = position + this->walkingHitBoxDelta;
    this->boundingBoxWalking->setPosition(boundingBoxWalkingPosition);
}

std::shared_ptr<StaticPolygon> Hiker::getCurrentBoundingBoxStatic() const {
    std::vector<Vector> vertices = this->boundingBoxWalking->getBodySpaceVertices();
    for (auto &point : vertices) {
        point += this->boundingBoxWalking->getPosition();
    }
    return std::make_shared<StaticPolygon>(vertices);
}

void Hiker::addHitInformation(const HitInformation &hit) { this->hitInformation.push_back(hit); }
