#include "physics.h"
#include "../components/particle_state.h"
#include "../entities/mountain.h"
#include "flecs.h"
#include "raylib.h"

constexpr float GRAVITATIONAL_CONSTANT = 1000.8;

void makeRock(const flecs::world &world, Position p, Velocity v, Radius r) {
    flecs::entity new_rock = world.entity();
    new_rock.set<Position>({p.x, p.y}).set<Velocity>({v.x, v.y})
        .set<Radius>({r.value}).add<Rock>();
}

void updateVelocity(flecs::iter it, Position *positions, Velocity *velocities) {
    for (auto i : it) {

        /*
        auto x_min = p.x - r->r;
        auto x_max = p.x + r->r;
        IndexInterval interval;
        interval = m->getRelevantMountainSection(x_min, x_max);

        for (int i = interval.start_indice; i == interval.end_indice; i++) {
            auto mountain_node = m->getFixpoint(i);


        }
        */

        velocities[i].y += GRAVITATIONAL_CONSTANT * it.delta_time();
    }
}

void updatePosition(flecs::iter it, Position *positions, Velocity *velocities) {
    for (auto i : it) {
        auto p = positions[i];
        auto v = velocities[i];

        // Update

        positions[i].x += v.x * it.delta_time();
        positions[i].y += v.y * it.delta_time();
    }
}

void debugRenderRocks(flecs::iter it, Position *positions, Radius *r) {
    for (auto i: it) {
        DrawCircle(positions[i].x, positions[i].y, r[i].value, RED);
    }
}

PhysicSystems::PhysicSystems(flecs::world &world) {
    world.module<PhysicSystems>();

    world.system<Position, Velocity>().with<Rock>().iter(updatePosition);
    world.system<Position, Velocity>().with<Rock>().iter(updateVelocity);

    world.system<Position, Radius>().with<Rock>().iter(debugRenderRocks);

    Position p {500., 500.};
    Velocity v {0., 0.};
    Radius r {20.0};
    makeRock(world, p, v, r);
}
