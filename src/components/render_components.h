#pragma once
#include "../utils/resource_manager.h"
#include <raylib.h>

namespace graphics {

struct SpriteComponent {
    int width;
    int height;
    HANDLE resourceHandle;
};


struct AnimatedSpriteComponent {
    HANDLE resourceHandle;
    HANDLE currentFrame;
};

struct ChunkRenderComponent {
    HANDLE resourceHandle;
};

struct BillboardComponent {
    Vector3 billUp;
    Vector3 billPositionStatic;

};

typedef Camera2D Camera2DComponent;

} // namespace graphics
