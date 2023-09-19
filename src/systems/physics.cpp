#include "physics.h"
#include "../components/particle_state.h"
#include "flecs.h"

void example_system(flecs::iter it, Position *positions, Velocity *velocities) {
    for (auto i : it) {
        auto p = positions[i];
        auto v = velocities[i];
        // Do sth
    }
}

PhysicSystems::PhysicSystems(flecs::world &world) {
    world.module<PhysicSystems>();

    world.system<Position, Velocity>().iter(example_system);
}
