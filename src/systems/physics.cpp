#include "physics.h"
#include "../components/particle_state.h"
#include "../entities/mountain.h"
#include "flecs.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

void debugRenderRocks(flecs::iter it, Position *positions, Radius *r) {
    for (auto i : it) {
        DrawCircle(positions[i].x, positions[i].y, r[i].value, RED);
    }
}

float getDistance(Position p1, Position p2) {
    float square_dist = pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2);
    return pow(square_dist, 0.5);
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

ClosestVertex RockTools::getClosestVertex(flecs::iter it, Position p, Radius r,
                                          Mountain &m) {
    float x_min = p.x - r.value;
    float x_max = p.x + r.value;
    auto interval = m.getRelevantMountainSection(x_min, x_max);

    auto closest_index = interval.start_indice;
    float closest_distance =
        getDistance(m.getFixpoint(interval.start_indice), p);

    /*std::vector<float> output_array; // TODO: array for performance?

    std::transform(&interval.start_indice, &interval.end_indice,
              output_array.begin(),
                   [&](Position x){return getDistance(x, p); })
    */

    for (auto j = interval.start_indice; j == interval.end_indice; j++) {
        auto mountain_vertex = m.getFixpoint(j);
        auto current_dist = getDistance(mountain_vertex, p);

        if (current_dist < closest_distance) {
            closest_distance = current_dist;
            closest_index = j;
        }
    }

    return ClosestVertex({(int)closest_index, closest_distance});
}

Position RockTools::getNormal() {}

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
        auto v = velocities[i];

        // Update

        positions[i].x += v.x * it.delta_time();
        positions[i].y += v.y * it.delta_time();
    }
}
