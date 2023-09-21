#include "physics.h"
#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "flecs.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

void debugRenderRocks(flecs::iter it, Position *positions, Radius *r) {
    for (auto i : it) {
        DrawCircle(positions[i].x, positions[i].y, r[i].value, RED);
    }
}

float getDistance(Vector p1, Vector p2) {
    Vector delta_pos = p1 - p2;
    return std::sqrt(delta_pos * delta_pos);
}

Velocity RockTools::reflectVelocity(Velocity velocity, Vector normal_vector) {
    float normal_velocity = std::abs(velocity * normal_vector);

    Vector v = velocity + normal_vector * normal_velocity * 2.f;
    return {v.x, v.y};
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

ClosestVertex RockTools::getClosestVertex(Position p, Radius r, Mountain &m) {
    float x_min = p.x - r.value;
    float x_max = p.x + r.value;
    auto interval = m.getRelevantMountainSection(x_min, x_max);

    auto closest_index = interval.start_index;
    float closest_distance = getDistance(m.getVertex(interval.start_index), p);

    /*std::vector<float> output_array; // TODO: array for performance?

    std::transform(&interval.start_index, &interval.end_index,
              output_array.begin(),
                   [&](Position x){return getDistance(x, p); })
    */

    for (auto j = interval.start_index; j == interval.end_index; j++) {
        auto mountain_vertex = m.getVertex(j);
        auto current_dist = getDistance(mountain_vertex, p);

        if (current_dist < closest_distance) {
            closest_distance = current_dist;
            closest_index = j;
        }
    }

    return ClosestVertex({closest_index, closest_distance});
}

Vector RockTools::getNormal(std::size_t idx, Position rock_pos, Mountain &m) {
    // determine closer vertex
    Position vertex_other = m.getVertex((idx - 1) % m.NUMBER_OF_VERTICES);
    Position vertex_right = m.getVertex((idx + 1) % m.NUMBER_OF_VERTICES);
    if (getDistance(rock_pos, vertex_other) >
        getDistance(rock_pos, vertex_right)) {
        vertex_other = vertex_right; // else already correct
    }
    // calc distances
    Position vertex = m.getVertex(idx);
    float d_x = vertex.x - vertex_other.x;
    float d_y = vertex.y - vertex_other.y;
    // compute normal from distances via rotation
    // signbit  is used to let normal vector always point in positive y
    // direction
    float sgn_n_x =
        (std::signbit(-d_y) - 0.5) * 2; // Hacky sign, that cpp missing
    float n_x = sgn_n_x * -d_y;         // R =  (  0   -1  )
    float n_y = sgn_n_x * d_x;          //      (  1    0  )
    float normalization = std::sqrt(n_x * n_x + n_y * n_y);
    n_x /= normalization;
    n_y /= normalization;
    return Vector{n_x, n_y};
}

void RockTools::terrainCollision(flecs::iter it, Position *positions,
                                 Velocity *velocities, Radius *r, Mountain &m) {
    for (auto i : it) {
        auto closest_vertex =
            RockTools::getClosestVertex(positions[i], r[i], m);
        auto vertex_position = m.getVertex(closest_vertex.index);
        if (closest_vertex.distance > r[i].value) {
            break;
        }
        auto normal_vector =
            RockTools::getNormal(closest_vertex.index, positions[i], m);
        velocities[i] =
            RockTools::reflectVelocity(velocities[i], normal_vector);

        float vertex_normal = vertex_position * normal_vector;
        float position_normal = positions[i] * normal_vector;
        float velocity_normal = velocities[i] * normal_vector;

        float terrain_exit_time =
            (r[i].value + vertex_normal - position_normal) / velocity_normal;
        float epsilon = 0.1;

        positions[i].x += velocities[i].x * terrain_exit_time + epsilon;
        positions[i].y += velocities[i].y * terrain_exit_time + epsilon;
    }
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
        auto v = velocities[i];

        // Update

        positions[i].x += v.x * it.delta_time();
        positions[i].y += v.y * it.delta_time();
    }
}
