//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Rock.h"

#include "../../utilities/vector.h"
#include <iostream>
#include <utility>

Rock::Rock(Vector position, Vector velocity, floatType angularVelocity, floatType angularOffset, floatType radius)
    : RenderedEntity(position), velocity(velocity), angularOffset(angularOffset), angularVelocity(angularVelocity),
      radius(radius){};

RenderInformation Rock::getRenderInformation() const {
    if (animation.frames == 0) {
        return RenderInformation{Vector2(position), {0, 0}, radius * 2.0f, radius * 2.0f, angularOffset, "rock"};
    } else {
        return RenderInformation{Vector2(position), {0, 0},      radius * 2.0f,  radius * 2.0f,
                                 angularOffset,     "explosion", {25, 0, 0.1, 0}};
    }
}

void Rock::setVelocity(Vector &newVelocity) { velocity = newVelocity; }

Vector Rock::getVelocity() { return velocity; }

floatType Rock::getRadius() const { return radius; }

bool Rock::getShouldBeDestroyed() const { return this->shouldBeDestroyed; }
void Rock::setShouldBeDestroyed(const bool shouldBeDestroyedNew) { this->shouldBeDestroyed = shouldBeDestroyedNew; }
void Rock::setAngularVelocity(const floatType newAngularVelocity) { this->angularVelocity = newAngularVelocity; }
void Rock::setAngularOffset(const floatType newAngularOffset) { this->angularOffset = newAngularOffset; }
floatType Rock::getAngularVelocity() const { return angularVelocity; }
floatType Rock::getAngularOffset() const { return angularOffset; }
