#include "physics.h"
#include "../components/particle_state.h"
#include "../entities/mountain.h"
#include "flecs.h"
#include "raylib.h"

void debugRenderRocks(flecs::iter it, Position *positions, Radius *r) {
    for (auto i : it) {
        DrawCircle(positions[i].x, positions[i].y, r[i].value, RED);
    }
}

PhysicSystems::PhysicSystems(flecs::world &world) {
    world.module<PhysicSystems>();

    world.system<Position, Velocity>().with<Rock>().iter(
        RockTools::updateState);

    world.system<Position, Radius>().with<Rock>().iter(debugRenderRocks);

    Position p{500., 500.};
    Velocity v{0., 0.};
    Radius r{20.0};
    RockTools::makeRock(world, p, v, r);
}

void RockTools::makeRock(const flecs::world &world, Position p, Velocity v,
                         Radius r) {

    flecs::entity new_rock = world.entity();
    new_rock.set<Position>({p.x, p.y})
        .set<Velocity>({v.x, v.y})
        .set<Radius>({r.value})
        .add<Rock>();
}

void RockTools::updateState(flecs::iter it, Position *positions,
                            Velocity *velocities) {
    updateVelocity(it, positions, velocities);
    updatePosition(it, positions, velocities);
}

void RockTools::updateVelocity(flecs::iter it, Position *positions,
                               Velocity *velocities) {
    for (auto i : it) {
        velocities[i].y += GRAVITATIONAL_CONSTANT * it.delta_time();
    }
}

void RockTools::updatePosition(flecs::iter it, Position *positions,
                               Velocity *velocities) {
    for (auto i : it) {
        auto p = positions[i];
        auto v = velocities[i];

        // Update

        positions[i].x += v.x * it.delta_time();
        positions[i].y += v.y * it.delta_time();
    }
}
