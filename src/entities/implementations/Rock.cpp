//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Rock.h"

#include "../../utilities/vector.h"
#include <cmath>
#include <iostream>
#include <utility>

Rock::Rock(const Vector &position, const std::vector<Vector> &vertices, const std::vector<Vector2> &textureCoordinates,
           const floatType mass, const floatType momentOfInertia, const DynamicProperties &dynamicProperties)
    : DynamicPolygon(position, vertices, textureCoordinates, mass, momentOfInertia, dynamicProperties) {}

RenderInformation Rock::getRenderInformation() const {
    if (animation.frames == 0) {
        return RenderInformation{Vector2(this->position),
                                 {0, 0},
                                 0.f,
                                 0.f,
                                 this->dynamicProperties.rotationAngleRad * static_cast<floatType>(180.0f / M_PI),
                                 "rockRectangle"};
    } else {
        return RenderInformation{Vector2(this->position),
                                 {0, 0},
                                 this->getBoundingBox().width,
                                 this->getBoundingBox().height,
                                 this->dynamicProperties.rotationAngleRad,
                                 "explosion",
                                 {25, 0, 0.1, 0}};
    }
}

bool Rock::getShouldBeDestroyed() const { return this->shouldBeDestroyed; }
void Rock::setShouldBeDestroyed(const bool shouldBeDestroyedNew) { this->shouldBeDestroyed = shouldBeDestroyedNew; }
