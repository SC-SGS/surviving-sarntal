#pragma once
#include "../utils/resource_manager.h"
#include <raylib.h>

namespace graphics {

struct SpriteComponent {
    int width;
    int height;
    HANDLE resourceHandle;
};

struct CircleShapeRenderComponent {
    float radius;
};

struct RectangleShapeRenderComponent {
    float width;
    float height;
};

struct AnimatedSpriteComponent {
    HANDLE resourceHandle;
    HANDLE current_frame;
};

struct ChunkRenderComponent {
    HANDLE resourceHandle;
};

struct AudioComponent {
    HANDLE resourceHandle;
};

struct BillboardComponent {
    Vector3 billUp;
    Vector3 billPositionStatic;
    int width;
    int height;
    HANDLE resourceHandle;
};
struct AnimatedBillboardComponent {
    Vector3 billUp;
    Vector3 billPositionStatic;
    int width;
    int height;
    HANDLE resourceHandle;
    int current_frame;
    int numFrames;
    int animation_speed = 1;
    double time_passed = 0.0f;
};

struct LifeTime {
    float time = 1.0f;
};

typedef Camera2D Camera2DComponent;

} // namespace graphics
