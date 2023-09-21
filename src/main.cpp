#include "systems/physics.h"
#include "systems/render_systems.h"
#include <flecs.h>
#include "raylib.h"
#include "entities/mountain.h"

int main() {
    std::cout << "surviving sarntal" << std::endl;

    flecs::world w;

    world.import <RenderSystems>();
    world.import <PhysicSystems>();
    auto mountain_entity = world.entity().add<Mountain>();

    main_loop(world);
}
