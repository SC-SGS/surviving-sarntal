#include "components/mountain.h"
#include "systems/physics.h"
#include "systems/render_systems.h"
#include <flecs.h>
#include <iostream>

int main() {
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;

    world.import <RenderSystems>();
    world.import <PhysicSystems>();

    world.set<Mountain>({});

    main_loop(world);
}
