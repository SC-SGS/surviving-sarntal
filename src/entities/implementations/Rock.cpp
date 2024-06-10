//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Rock.h"

#include "../../utilities/vector.h"
#include <iostream>
#include <utility>

RenderInformation Rock::getRenderInformation() {
    return RenderInformation{Vector2(position), {0, 0}, radius * 2.0f, radius * 2.0f, rotation, "rock"};
}

void Rock::setRotation(const Rotation &newRotation) { rotation = newRotation; }

void Rock::setVelocity(Vector &newVelocity) { velocity = newVelocity; }

Rotation Rock::getRotation() { return rotation; }

Vector Rock::getVelocity() { return velocity; }

floatType Rock::getRadius() const { return radius; }

bool Rock::getShouldBeDestroyed() const { return this->shouldBeDestroyed; }
void Rock::setShouldBeDestroyed(const bool shouldBeDestroyed) { this->shouldBeDestroyed = shouldBeDestroyed; }
