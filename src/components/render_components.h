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
    HANDLE currentFrame;
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
    HANDLE currentFrame;
    int numFrames;
};

typedef Camera2D Camera2DComponent;

} // namespace graphics
