//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Hiker.h"
#include "../../output/haptics/HapticsService.hpp"
#include "spdlog/spdlog.h"
#include <iostream>

Hiker::Hiker(const Vector position, AudioService &audioService, HikerConstants hikerConstants)
    : RenderedEntity(position), hikerConstants(hikerConstants), audioService(audioService) {

    velocity = {0, 0};
    height = hikerConstants.hikerHeight;
    width = hikerConstants.hikerWidth;
    healthPoints = hikerConstants.hikerMaxHealth;
    hikerMovement = HikerMovement();
    isAlive = true;
    animation = {4, 0, 0.3, 0};

    spdlog::info("A Hiker was initialized");
}

RenderInformation Hiker::getRenderInformation() const {
    floatType directedWidth = width * static_cast<float>(hikerMovement.getDirection() != HikerMovement::LEFT ? 1 : -1);
    return RenderInformation{
        Vector2(position), {0, height / 2}, directedWidth, height, 0, hikerMovement.getStateString(), animation};
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

const HitInformation &Hiker::getHitInformation() const { return hitInformation; }

void Hiker::setHitInformation(const struct HitInformation &hit) { hitInformation = hit; }

bool Hiker::getIsHit() const { return isHit; }

void Hiker::setIsHit(bool isHit) { this->isHit = isHit; }

const Vector &Hiker::getVelocity() const { return velocity; }

void Hiker::setVelocity(const Vector &newVel) { velocity = newVel; }

bool Hiker::getIsAlive() const { return this->isAlive; }

void Hiker::setIsAlive(bool alive) { isAlive = alive; }
void Hiker::addHealthPoints(const int points) {
    this->setHealthPoints(std::min(this->healthPoints + points, hikerConstants.hikerMaxHealth));
}

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
void Hiker::moveToRight(floatType deltaX) { this->position.setX(this->position.x + deltaX); }
void Hiker::moveToLeft(floatType deltaX) { this->position.setX(this->position.x - deltaX); }
void Hiker::accelerateX(floatType deltaX) { this->velocity.setX(this->velocity.x + deltaX); }
void Hiker::accelerateY(floatType deltaY) { this->velocity.setY(this->velocity.y + deltaY); }
void Hiker::setXVelocity(floatType xValue) { this->velocity.setX(xValue); }
void Hiker::setYVelocity(floatType yValue) { this->velocity.setY(yValue); }
void Hiker::setLastJump(float lastJump) { this->hikerMovement.setLastJump(lastJump); }
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