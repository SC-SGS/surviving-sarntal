//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "Rock.h"

RenderInformation RockClass::getRenderInformation() {
    return RenderInformation{Vector2(position), radius, radius, rotation, "rock"};
}

void RockClass::setRotation(Rotation &newRotation) { rotation = newRotation; }

void RockClass::setVelocity(Vector &newVelocity) { velocity = newVelocity; }

Rotation RockClass::getRotation() { return rotation; }

Vector RockClass::getVelocity() { return velocity; }

float RockClass::getRadius() const { return radius; }
