#pragma once

#include "flecs.h"
#include "string"
#include <raylib.h>

namespace graphics {
constexpr char WINDOW_NAME[] = "Surviving Sarntal";
constexpr bool DEBUG = true;

void init_render_system(flecs::world &world);
void render_system(flecs::iter &iter);
Mesh generate_chunk_mesh();
void destroy();
} // namespace graphics
