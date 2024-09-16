//
// Created by Anietta Weckauff on 07.05.24.
//

#ifndef SURVIVING_SARNTAL_RENDERINFORMATION_H
#define SURVIVING_SARNTAL_RENDERINFORMATION_H

#include "../../../game/GameProperties.hpp"
#include "../../../utilities/vector.h"
#include "AnimationInformation.h"
#include "raylib.h"
#include <string>

/**
 * This struct contains all the information necessary for rendering a rectangular entity
 * of our game world.
 */
struct RenderInformation {
    Vector2 position;
    Vector2 offset = {0, 0};
    floatType width;
    floatType height;
    floatType rotationAngle;
    std::string texture;
    AnimationInformation animation = {0, 0, 0, 0};
};

#endif // SURVIVING_SARNTAL_RENDERINFORMATION_H
