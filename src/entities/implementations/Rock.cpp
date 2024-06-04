//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Rock.h"

#include "../../components/vector.h"
#include <iostream>
#include <utility>

RenderInformation RockClass::getRenderInformation() {
    return RenderInformation{Vector2(position), {0, 0}, radius * 2.0f, radius * 2.0f, rotation, "rock"};
}

void RockClass::setRotation(const Rotation &newRotation) { rotation = newRotation; }

void RockClass::setVelocity(Vector &newVelocity) { velocity = newVelocity; }

Rotation RockClass::getRotation() { return rotation; }

Vector RockClass::getVelocity() { return velocity; }

float RockClass::getRadius() const { return radius; }

bool RockClass::getShouldBeDestroyed() const { return this->shouldBeDestroyed; }
void RockClass::setShouldBeDestroyed(const bool shouldBeDestroyed) { this->shouldBeDestroyed = shouldBeDestroyed; }
