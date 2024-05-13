//
// Created by Anietta Weckauff on 07.05.24.
//

#ifndef SURVIVING_SARNTAL_RENDERINFORMATION_H
#define SURVIVING_SARNTAL_RENDERINFORMATION_H

#include "../utils/resource_manager.h"
#include "raylib.h"

struct RenderInformation {
    Vector3 billUp;
    Vector3 billPositionStatic;
    int width;
    int height;
    graphics::HANDLE resourceHandle;
};

#endif // SURVIVING_SARNTAL_RENDERINFORMATION_H
