#pragma once

#include "flecs.h"
#include "string"

namespace graphics {
    constexpr char WINDOW_NAME[] = "Surviving Sarntal";

    struct RenderSystems {
        explicit RenderSystems(flecs::world &world);

        ~RenderSystems();

        void init();

        void destroy();

    };

    

}

