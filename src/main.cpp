#include "systems/physics.h"
#include "systems/render_systems.h"
#include <flecs.h>
#include <iostream>
#include <raylib.h>

void main_loop(flecs::world &world);

int main() {
    flecs::world world;

    world.import <graphics::RenderSystems>();
    world.import <PhysicSystems>();

    main_loop(world);
}

void main_loop(flecs::world &world) {


    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        world.progress(dt);
    }

}