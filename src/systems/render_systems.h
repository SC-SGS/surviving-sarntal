#pragma once

#include "flecs.h"
#include "string"
#include <raylib.h>

namespace graphics {

constexpr char WINDOW_NAME[] = "Surviving Sarntal";
constexpr bool DEBUG = true;

// to be called once at app startup
void initRenderSystem(const flecs::world &world);

// prepares resources, call before render*
void prepareGameResources(const flecs::world &world);
void prepareMenuResources(const flecs::world &world);

// pre rendering setup per frame
void startRender(const flecs::iter &iter);
void endRender(const flecs::iter &iter);

// call per frame to render the current frame
void renderSystem(const flecs::iter &iter);
void renderHUD(const flecs::iter &iter);
void renderMenu(const flecs::iter &iter);

// helpers
void generateChunkMesh(const flecs::world &world);
Vector3 computeNormal(Vector3 p1, Vector3 p2, Vector3 p3);
void handleWindow(flecs::world &world);
void destroy();

} // namespace graphics
