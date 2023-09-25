#pragma once

#include "flecs.h"
#include "string"
#include <raylib.h>

namespace graphics {
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
#define MAX_INSTANCES 1000

constexpr char WINDOW_NAME[] = "Surviving Sarntal";
constexpr bool DEBUG = true;
static bool s_regenerateTerrain = true;

struct GenChunkEvent {};

void init_render_system(flecs::world &world);
void render_system(flecs::iter &iter);
Mesh generate_chunk_mesh(flecs::world &world);
Vector3 compute_normal(Vector3 p1, Vector3 p2, Vector3 p3);
void destroy();
} // namespace graphics
