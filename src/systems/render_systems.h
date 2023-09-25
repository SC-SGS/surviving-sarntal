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

void initRenderSystem(flecs::world &world);
void renderSystem(flecs::iter &iter);
Mesh generateChunkMesh(flecs::world &world);
Vector3 computeNormal(Vector3 p1, Vector3 p2, Vector3 p3);
void destroy();

} // namespace graphics
