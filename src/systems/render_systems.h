#pragma once

#include "flecs.h"
#include "string"
#include <raylib.h>

namespace graphics {
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 450;

constexpr char WINDOW_NAME[] = "Surviving Sarntal";
constexpr bool DEBUG = true;

void init_render_system(flecs::world &world);
void render_system(flecs::iter &iter);
Mesh generate_chunk_mesh(flecs::world &world, Vector2 &min, Vector2 &max);
Vector3 compute_normal(Vector3 p1, Vector3 p2, Vector3 p3);
void destroy();
} // namespace graphics
