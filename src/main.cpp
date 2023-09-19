#include "systems/physics.h"
#include "systems/render_systems.h"
#include <flecs.h>
#include <iostream>
#include <raylib.h>

int main() {
    flecs::world world;

    world.import <RenderSystems>();
    world.import <PhysicSystems>();

    main_loop(world);
}
