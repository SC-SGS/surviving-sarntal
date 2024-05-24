//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "Hiker.h"

#include "../components/vector.h"
#include "../utils/game_constants.h"
#include "iostream"
#include <utility>

Hiker::Hiker(Vector position)
    : RenderedEntity(position), height(HIKER_HEIGHT), width(HIKER_WIDTH), healthPoints(HIKER_MAX_HEALTH),
      hikerMovement(HikerMovement()), isHit(false), hitInformation(), velocity({0, 0}), isAlive(true) {}

RenderInformation Hiker::getRenderInformation() {
    return RenderInformation{Vector2(position), width, height, {0, 0}, hikerMovement.getStateString()};
}

float Hiker::getHeight() const { return height; }

void Hiker::setHeight(float h) { height = h; }

float Hiker::getWidth() const { return width; }

void Hiker::setWidth(float w) { width = w; }

int Hiker::getHealthPoints() const { return healthPoints; }

void Hiker::setHealthPoints(int hp) { healthPoints = hp; }

HikerMovement Hiker::getHikerMovement() const { return hikerMovement; }

void Hiker::setHikerMovement(const HikerMovement &movement) {
    switch (movement.getState()) {
    case HikerMovement::MOVING:
        height = HIKER_HEIGHT;
        width = HIKER_WIDTH;
        break;
    case HikerMovement::DUCKED:
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

HitInformation Hiker::getHitInformation() const { return hitInformation; }

void Hiker::setHitInformation(const struct HitInformation &hit) { hitInformation = hit; }

bool Hiker::getIsHit() const { return isHit; }

void Hiker::setIsHit(bool isHit) { this->isHit = isHit; }

Vector Hiker::getVelocity() const { return velocity; }

void Hiker::setVelocity(const Vector &newVel) { velocity = newVel; }
bool Hiker::getIsAlive() const { return isAlive; }

void Hiker::setIsAlive(bool alive) { isAlive = alive; }