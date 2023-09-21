#include "components/particle_state.h"
#include "entities/mountain.h"
#include "raylib.h"
#include "systems/physics.h"
#include "systems/render_systems.h"
#include <flecs.h>
#include <iostream>

void main_loop(flecs::world &world);

int main() {
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;

    graphics::init_render_system(world);

    world.import <PhysicSystems>();

    world.set<Mountain>({});

    world.set<AppInfo>({});

    main_loop(world);

    graphics::destroy();
}

void main_loop(flecs::world &world) {
    world.progress(0);

    SetTargetFPS(60);

    auto appInfo = world.get<AppInfo>();

    while (appInfo->isRunning) {
        float dt = GetFrameTime();
        world.progress(dt);
    }
}
