//
// Created by Anietta Weckauff on 07.05.24.
//

#ifndef SURVIVING_SARNTAL_RENDERINFORMATION_H
#define SURVIVING_SARNTAL_RENDERINFORMATION_H

#include "../../components/vector.h"
#include "../../utils/resource_manager.h"
#include "raylib.h"

struct RenderInformation {
    Vector2 position;
    float width;
    float height;
    Rotation rotation;
    std::string texture;
};

#endif // SURVIVING_SARNTAL_RENDERINFORMATION_H
