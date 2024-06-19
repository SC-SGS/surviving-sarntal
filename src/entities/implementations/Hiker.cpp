//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Hiker.h"
#include "../../output/haptics/HapticsService.hpp"
#include "../../utilities/GameConstants.hpp"
#include "../../utilities/vector.h"
#include "../World.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <utility>

#include <memory>

Hiker::Hiker(const Vector position)
    : RenderedEntity(position), velocity({0, 0}), height(HIKER_HEIGHT), width(HIKER_WIDTH),
      healthPoints(HIKER_MAX_HEALTH), hikerMovement(HikerMovement()), isAlive(true) {
    animation = {4, 0, 0.3, 0};
    spdlog::info("A Hiker was initialized");
}

RenderInformation Hiker::getRenderInformation() {
    return RenderInformation{Vector2(position), {0, height / 2}, width, height, {0, 0}, hikerMovement.getStateString(),
                             animation};
}

floatType Hiker::getHeight() const { return height; }

void Hiker::setHeight(const floatType height) { this->height = height; }

floatType Hiker::getWidth() const { return width; }

void Hiker::setWidth(const floatType width) { this->width = width; }

int Hiker::getHealthPoints() const { return healthPoints; }

void Hiker::setHealthPoints(const int healthPoints) { this->healthPoints = healthPoints; }

const HikerMovement &Hiker::getHikerMovement() const { return hikerMovement; }

void Hiker::setHikerMovement(const HikerMovement &movement) {
    switch (movement.getState()) {
    case HikerMovement::MOVING:
        height = HIKER_HEIGHT;
        width = HIKER_WIDTH;
        break;
    case HikerMovement::CROUCHED:
        height = DUCKED_HIKER_HEIGHT;
        width = DUCKED_HIKER_WIDTH;
        break;
    case HikerMovement::IN_AIR:
        height = HIKER_HEIGHT;
        width = HIKER_WIDTH;
        break;
    }
    hikerMovement = movement;
}

const HitInformation &Hiker::getHitInformation() const { return hitInformation; }

void Hiker::setHitInformation(const struct HitInformation &hit) { hitInformation = hit; }

bool Hiker::getIsHit() const { return isHit; }

void Hiker::setIsHit(bool isHit) { this->isHit = isHit; }

const Vector &Hiker::getVelocity() const { return velocity; }

void Hiker::setVelocity(const Vector &newVel) { velocity = newVel; }

bool Hiker::getIsAlive() const { return this->isAlive; }

void Hiker::setIsAlive(bool alive) { isAlive = alive; }
void Hiker::addHealthPoints(const int points) {
    this->setHealthPoints(std::min(this->healthPoints + points, HIKER_MAX_HEALTH));
}

void Hiker::turnLeft() { this->hikerMovement.setDirection(HikerMovement::LEFT); }
void Hiker::turnRight() { this->hikerMovement.setDirection(HikerMovement::RIGHT); }
void Hiker::turnNeutral() { this->hikerMovement.setDirection(HikerMovement::NEUTRAL); }
void Hiker::crouch() {
    if (this->hikerMovement.getState() == HikerMovement::MOVING) {
        audioService.playSound("crouch");
        this->hikerMovement.setState(HikerMovement::CROUCHED);
        this->setHeight(DUCKED_HIKER_HEIGHT);
        this->setWidth(DUCKED_HIKER_WIDTH);
    }
}
void Hiker::uncrouch() {
    if (this->hikerMovement.getState() == HikerMovement::CROUCHED) {
        hikerMovement.setState(HikerMovement::MOVING);
        this->setHeight(HIKER_HEIGHT);
        this->setWidth(HIKER_WIDTH);
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
void Hiker::moveToRight(floatType deltaX) { this->position.setX(this->position.x + deltaX); }
void Hiker::moveToLeft(floatType deltaX) { this->position.setX(this->position.x - deltaX); }
void Hiker::accelerateX(floatType deltaX) { this->velocity.setX(this->velocity.x + deltaX); }
void Hiker::accelerateY(floatType deltaY) { this->velocity.setY(this->velocity.y + deltaY); }
void Hiker::setXVelocity(floatType xValue) { this->velocity.setX(xValue); }
void Hiker::setYVelocity(floatType yValue) { this->velocity.setY(yValue); }
void Hiker::setLastJump(float lastJump) { this->hikerMovement.setLastJump(lastJump); }
void Hiker::doSecondJump() {
    audioService.playSound("jump");
    this->velocity.setY(JUMP_VELOCITY_CONSTANT);
    if (this->hikerMovement.getState() == HikerMovement::IN_AIR) {
        this->hikerMovement.setCanJumpAgain(false);
    }
    hikerMovement.setState(HikerMovement::IN_AIR);
}

bool Hiker::needsToDie() const {
    return this->getPosition().x - World::getInstance().getMonster().getXPosition() <= 0 || this->healthPoints <= 0;
}

void Hiker::kill() {
    this->isAlive = false;
    this->healthPoints = 0;
    this->velocity = {0, 0};
    HapticsService::deathRumble();
}
