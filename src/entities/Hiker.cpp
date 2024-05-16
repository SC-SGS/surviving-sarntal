//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "Hiker.h"
#include "../components/vector.h"
#include "../utils/game_constants.h"
#include "iostream"

Hiker::Hiker()
    : height(0), width(0), healthPoints(100), hikerMovement(), isHit(false), hitInformation(), velocity(),
      isAlive(true) {}

Hiker::Hiker(HikerMovement hikerMovement, Vector velocity)
    : height(0.0f), width(0.0f), healthPoints(100), hikerMovement(hikerMovement), isHit(false), hitInformation(),
      velocity(velocity), isAlive(true) {}

float Hiker::getHeight() const { return height; }

void Hiker::setHeight(float h) { height = h; }

float Hiker::getWidth() const { return width; }

void Hiker::setWidth(float w) { width = w; }

int Hiker::getHealthPoints() const { return healthPoints; }

void Hiker::setHealthPoints(int hp) { healthPoints = hp; }

HikerMovement Hiker::getHikerMovement() const { return hikerMovement; }

void Hiker::setHikerMovement(const HikerMovement &movement) { hikerMovement = movement; }

HitInformation Hiker::getHitInformation() const { return hitInformation; }

void Hiker::setHitInformation(const struct HitInformation &hit) { hitInformation = hit; }

bool Hiker::getIsHit() const { return isHit; }

void Hiker::setIsHit(bool isHitted) { isHit = isHitted; }

Vector Hiker::getVelocity() const { return velocity; }

void Hiker::setVelocity(const Vector &newVel) { velocity = newVel; }
bool Hiker::getIsAlive() const { return isAlive; }

void Hiker::setIsAlive(bool alive) { isAlive = alive; }
