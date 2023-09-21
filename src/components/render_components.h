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

typedef Camera2D Camera2DComponent;

} // namespace graphics
