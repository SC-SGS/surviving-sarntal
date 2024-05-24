//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "Rock.h"

#include "../components/vector.h"
#include <utility>

RockClass::RockClass(Vector velocity, Rotation rotation, float radius, Vector position)
    : RenderedEntity(position), velocity(velocity), rotation(rotation), radius(radius) {}

RenderInformation RockClass::getRenderInformation() {
    return RenderInformation{Vector2(position), radius, radius, rotation, "rock"};
}

void RockClass::setRotation(Rotation &newRotation) { rotation = newRotation; }

void RockClass::setVelocity(Vector &newVelocity) { velocity = newVelocity; }

Rotation RockClass::getRotation() { return rotation; }

Vector RockClass::getVelocity() { return velocity; }

float RockClass::getRadius() const { return radius; }
