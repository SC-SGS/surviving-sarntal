#include <iostream>
#include <raylib.h>
#include <flecs.h>
#include "systems/render_systems.h"

int main() {
    flecs::world world;

    world.import <RenderSystems>();

    // hi

    main_loop(world);
}
