#pragma once

#include "flecs.h"
#include "string"

namespace graphics {
constexpr char WINDOW_NAME[] = "Surviving Sarntal";
void init_render_system(flecs::world &world);
void render_system(flecs::iter iter);
void destroy();
} // namespace graphics
