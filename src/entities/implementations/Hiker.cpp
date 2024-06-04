//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Hiker.h"
#include "../../components/vector.h"
#include "../../utils/game_constants.hpp"
#include "../World.h"
#include <iostream>
#include <utility>

#include <memory>

Hiker::Hiker(const Vector position)
    : RenderedEntity(position), velocity({0, 0}), height(HIKER_HEIGHT), width(HIKER_WIDTH),
      healthPoints(HIKER_MAX_HEALTH), hikerMovement(HikerMovement()), isAlive(true) {
    animation = {4, 0, 0.3, 0};
    std::cout << "Hiker initialized." << std::endl;
}

RenderInformation Hiker::getRenderInformation() {
    return RenderInformation{Vector2(position), {0, height / 2}, width, height, {0, 0}, hikerMovement.getStateString(),
                             animation};
}

float Hiker::getHeight() const { return height; }

void Hiker::setHeight(const float height) { this->height = height; }

float Hiker::getWidth() const { return width; }

void Hiker::setWidth(const float width) { this->width = width; }

int Hiker::getHealthPoints() const { return healthPoints; }

void Hiker::setHealthPoints(const int healthPoints) { this->healthPoints = healthPoints; }

HikerMovement &Hiker::getHikerMovement() { return hikerMovement; }

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

HitInformation &Hiker::getHitInformation() { return hitInformation; }

void Hiker::setHitInformation(const struct HitInformation &hit) { hitInformation = hit; }

bool Hiker::getIsHit() const { return isHit; }

void Hiker::setIsHit(bool isHit) { this->isHit = isHit; }

Vector &Hiker::getVelocity() { return velocity; }

void Hiker::setVelocity(const Vector &newVel) { velocity = newVel; }
bool Hiker::getIsAlive() {
    return this->getPosition().x - World::getInstance().getMonster().getXPosition() > 0 && this->healthPoints > 0;
}

void Hiker::setIsAlive(bool alive) { isAlive = alive; }
