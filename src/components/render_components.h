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

struct AnimatedSpriteComponent {
    HANDLE resourceHandle;
    HANDLE currentFrame;
};

struct ChunkRenderComponent {
    HANDLE resourceHandle;
};

struct AudioComponent{
    HANDLE resourceHandle;
};

typedef Camera2D Camera2DComponent;

} // namespace graphics
