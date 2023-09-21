#pragma once

#include "flecs.h"
#include "string"

namespace graphics {
constexpr char WINDOW_NAME[] = "Surviving Sarntal";
constexpr bool DEBUG = true;

struct RenderSystems {
    explicit RenderSystems(flecs::world &world);

    ~RenderSystems();

        void init(flecs::world &world);

    void destroy();
};

} // namespace graphics
