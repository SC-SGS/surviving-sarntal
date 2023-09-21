#include "components/mountain.h"
#include "raylib.h"
#include "systems/game_logic.h"
#include "systems/physics.h"
#include "systems/render_systems.h"
#include <flecs.h>
#include <iostream>

void main_loop(flecs::world &world);

int main() {
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;

    world.import <graphics::RenderSystems>();
    world.import <PhysicSystems>();

    initGameLogic(world);

    world.set<Mountain>({});

    main_loop(world);
}

void main_loop(flecs::world &world) {

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        world.progress(dt);
    }
}
