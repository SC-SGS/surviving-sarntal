#pragma once
#include <raylib.h>

namespace graphics {

struct SpriteComponent {
    int resourceHandle;
};

struct AnimatedSpriteComponent {
    int resourceHandle;
    int currentFrame;
};

struct ChunkRenderComponent {
    int resourceHandle;
};

typedef Camera2DComponent Camera2D;

} // namespace graphics
