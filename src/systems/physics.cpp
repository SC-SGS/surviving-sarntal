#include "physics.h"
#include "../components/mountain.h"
#include "../components/render_components.h"
#include "../components/vector.h"
#include "flecs.h"
#include "iostream"
#include "raylib.h"
#include <algorithm>
#include <cmath>

using namespace physics;

physics::ClosestVertex physics::getClosestVertex(Position p, Radius r,
                                                 Mountain *m) {
    float_type x_min = p.x - r.value;
    float_type x_max = p.x + r.value;
    auto interval = m->getRelevantMountainSection(x_min, x_max);

    auto closest_index = interval.start_index;
    float_type closest_distance =
        m->getVertex(interval.start_index).distanceTo(p);

    for (auto j = interval.start_index; j < interval.end_index; j++) {
        auto mountain_vertex = m->getVertex(j);
        auto current_dist = mountain_vertex.distanceTo(p);

        if (current_dist < closest_distance) {
            closest_distance = current_dist;
            closest_index = j;
        }
    }

    return ClosestVertex({closest_index, closest_distance});
}

Vector physics::getNormal(std::size_t idx, Position rock_pos, Mountain *m) {
    // determine closer vertex
    Position vertex_other =
        m->getVertex((idx - 1) % Mountain::NUMBER_OF_VERTICES);
    Position vertex_right =
        m->getVertex((idx + 1) % Mountain::NUMBER_OF_VERTICES);
    if (rock_pos.distanceTo(vertex_other) > rock_pos.distanceTo(vertex_right)) {
        // else already correct
        vertex_other = vertex_right;
    }
    // calc distances
    Position vertex = m->getVertex(idx);
    Vector d = vertex - vertex_other;
    // compute normal from distances via rotation
    // signbit is used to let normal vector always point in positive y direction
    float_type sgn_n_x = ((float_type)std::signbit(-d.y) - 0.5f) * 2.f;
    // R =  (  0   -1  )
    //      (  1    0  )
    Vector n = {sgn_n_x * -d.y, sgn_n_x * d.x};
    float_type normalization = std::sqrt(n * n);
    return n / normalization;
}

bool physics::isCollided(Position p1, Position p2, Radius r1, Radius r2) {
    return (p1.distanceTo(p2) <= r1.value + r2.value);
}

void physics::terrainCollision(flecs::iter it, Position *positions,
                               Velocity *velocities, Radius *r, Mountain *m) {
    for (auto i : it) {
        auto closest_vertex = getClosestVertex(positions[i], r[i], m);
        auto vertex_position = m->getVertex(closest_vertex.index);
        if (closest_vertex.distance > r[i].value) {
            continue;
        }
        auto normal_vector = getNormal(closest_vertex.index, positions[i], m);
        velocities[i] = (Velocity)velocities[i].reflectOnNormal(normal_vector);

        float_type vertex_normal = vertex_position * normal_vector;
        float_type position_normal = positions[i] * normal_vector;
        float_type velocity_normal = velocities[i] * normal_vector;

        float_type terrain_exit_time =
            (r[i].value + vertex_normal - position_normal) / velocity_normal;

        positions[i] += velocities[i] * terrain_exit_time + EPSILON;
    }
}

void physics::makeRock(const flecs::world &world, Position p, Velocity v,
                       float_type radius) {
    world.entity()
        .set<Position>(p)
        .set<Velocity>(v)
        .set<Radius>({radius})
        .add<Rock>()
        .set<graphics::CircleShapeRenderComponent>({radius});
}

void physics::rockCollision(Position &p1, Position &p2, Velocity &v1,
                            Velocity &v2, const Radius r1, const Radius r2) {
    float_type m1 = r1.value * r1.value;
    float_type m2 = r2.value * r2.value;
    float_type radius_sum = r1.value + r2.value;

    Vector vel_diff_vector = v1 - v2;
    Vector pos_diff_vector = p1 - p2;
    float_type distance_sq = pos_diff_vector * pos_diff_vector;
    Vector normal_vector =
        pos_diff_vector / (distance_sq + EPSILON);

    // Velocity Update
    float_type total_mass = m1 + m2;
    v1 -= pos_diff_vector * 2 * m2 * (vel_diff_vector * pos_diff_vector)
          / (distance_sq * total_mass + EPSILON);
    v2 += pos_diff_vector * 2 * m1 * (vel_diff_vector * pos_diff_vector)
          / (distance_sq * total_mass + EPSILON);

    // Position update using new velocities

    std::cout << v1.x << " " << v1.y << std::endl;
    std::cout << normal_vector.x << " " << normal_vector.y << std::endl;

    float_type normal_v1 = std::abs(v1 * normal_vector);
    float_type normal_v2 = std::abs(v2 * normal_vector);

    float_type overlap = radius_sum - pos_diff_vector.length() + EPSILON;
    float_type overlap1 = overlap * m2 / (m1 + m2);
    float_type overlap2 = overlap * m1 / (m1 + m2);

    float_type exit_time1 = overlap1 / (normal_v1 + EPSILON);
    float_type exit_time2 = overlap2 / (normal_v2 + EPSILON);

    p1 += v1 * exit_time1;
    p2 += v2 * exit_time2;
}

void physics::rockRockInteractions(flecs::iter it, Position *positions,
                                   Velocity *velocities, Radius *radius) {
    for (int i = 0; i < it.count(); i++) {
        for (int j = i + 1; j < it.count(); j++) {
            if (isCollided(positions[i], positions[j], radius[i], radius[j])) {
                rockCollision(positions[i], positions[j], velocities[i],
                              velocities[j], radius[i],
                              radius[j]); // TODO: Optimization?
            }
        }
    }
}

void physics::updateState(flecs::iter it, Position *positions,
                          Velocity *velocities) {
    updateVelocity(it, positions, velocities);
    updatePosition(it, positions, velocities);
}

void physics::updateVelocity(flecs::iter it, Position *positions,
                             Velocity *velocities) {
    for (auto i : it) {
        velocities[i].y += GRAVITATIONAL_CONSTANT * it.delta_time();
    }
}

void physics::updatePosition(flecs::iter it, Position *positions,
                             Velocity *velocities) {
    std::cout << 1. / it.delta_time() << std::endl;

    for (auto i : it) {
        positions[i] += velocities[i] * it.delta_time();
    }
}

PhysicSystems::PhysicSystems(flecs::world &world) {
    world.module<PhysicSystems>();



    world.system<Position, Velocity>().with<Rock>().iter(updateState);

    world.system<Position, Velocity, Radius>().with<Rock>().iter(
        rockRockInteractions);

    world.system<Position, Velocity, Radius, Mountain>()
        .term_at(4)
        .singleton()
        .iter(terrainCollision);

    for (int i = 0; i < 30; i++) {
        Position p{300.f - i * 20.f, 200.f + i * 25.f};
        Velocity v{0., 0.};
        makeRock(world, p, v, 10.f);
    }
}
