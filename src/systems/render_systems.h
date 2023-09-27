#pragma once

#include "flecs.h"
#include "string"
#include <raylib.h>

namespace graphics {

constexpr char WINDOW_NAME[] = "Surviving Sarntal";
constexpr bool DEBUG = true;

void initRenderSystem(const flecs::world &world);
void renderSystem(flecs::iter &iter);
void generateChunkMesh(const flecs::world &world);
Vector3 computeNormal(Vector3 p1, Vector3 p2, Vector3 p3);
void handleWindow(flecs::world &world);
void destroy();

} // namespace graphics
