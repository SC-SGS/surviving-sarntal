#pragma once

#include "flecs.h"
#include "string"

constexpr char WINDOW_NAME[] = "Surviving Sarntal";

struct RenderSystems {
    explicit RenderSystems(flecs::world &world);
};

void main_loop(flecs::world &world);
