#ifndef SURVIVING_SARNTAL_RENDER_SYSTEMS_H
#define SURVIVING_SARNTAL_RENDER_SYSTEMS_H

#include "flecs.h"
#include "string"

constexpr char WINDOW_NAME[] = "Surviving Sarntal";

struct RenderSystems {
    explicit RenderSystems(flecs::world &world);
};

void main_loop(flecs::world &world);

#endif // SURVIVING_SARNTAL_RENDER_SYSTEMS_H
