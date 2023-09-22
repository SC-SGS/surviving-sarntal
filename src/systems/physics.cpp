#include "physics.h"
#include "../components/mountain.h"
#include "../components/vector.h"
#include "flecs.h"
#include "iostream"
#include "raylib.h"
#include <algorithm>
#include <cmath>

using namespace physics;

void clear(flecs::iter) { ClearBackground(WHITE); }

void debugRenderRocks(flecs::iter it, Position *positions, Radius *r,
                      Mountain *m) {
    for (auto i : it) {
        DrawCircle(positions[i].x, positions[i].y, r[i].value, RED);
    }

    for (int i = 0; i < 1000; i++) {
        Position p = m->getVertex(i);
        DrawCircle(p.x, p.y, 5, RED);
    }
}

physics::ClosestVertex physics::getClosestVertex(Position p, Radius r,
                                                 Mountain *m) {
    float_type x_min = p.x - r.value;
    float_type x_max = p.x + r.value;
    auto interval = m->getRelevantMountainSection(x_min, x_max);

    auto closest_index = interval.start_index;
    float_type closest_distance =
        m->getVertex(interval.start_index).distanceTo(p);

    /*std::vector<float> output_array; // TODO: array for performance?

    std::transform(&interval.start_index, &interval.end_index,
              output_array.begin(),
                   [&](Position x){return getDistance(x, p); })
    */

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
    float_type d_x = vertex.x - vertex_other.x;
    float_type d_y = vertex.y - vertex_other.y;
    // compute normal from distances via rotation
    // signbit is used to let normal vector always point in positive y direction
    float_type sgn_n_x = ((float_type)std::signbit(-d_y) - 0.5f) * 2.f;
    // R =  (  0   -1  )
    //      (  1    0  )
    Vector n = {sgn_n_x * -d_y, sgn_n_x * d_x};
    float_type normalization = std::sqrt(n * n);
    return n / normalization;
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
        .add<Rock>();
}

void RockTools::rockCollision(Position& p1, Position& p2, Velocity& v1,
                              Velocity& v2, Radius r1, Radius r2) {
    float m1 = r1.value * r1.value;
    float m2 = r2.value * r2.value;
    float radius_sum = r1.value + r2.value;

    Vector vel_diff_vector = v1 - v2;
    Vector pos_diff_vector = p1 - p2;
    Vector normal_vector =
        pos_diff_vector * (1 / (pos_diff_vector * pos_diff_vector));

    // Wikipedia:
    Vector exit_vel1 = v1 - pos_diff_vector * (2 * m2 / (m1 + m2)) *
                                ((vel_diff_vector * pos_diff_vector) /
                                 (pos_diff_vector * pos_diff_vector));
    Vector exit_vel2 = v2 + pos_diff_vector * (2 * m1 / (m1 + m2)) *
                                ((vel_diff_vector * pos_diff_vector) /
                                 (pos_diff_vector * pos_diff_vector));

    float normal_v1 = exit_vel1 * normal_vector;
    float normal_v2 = exit_vel2 * normal_vector;
    float normal_p1 = p1 * normal_vector;
    float normal_p2 = p2 * normal_vector;

    float overlap = std::abs(normal_p1 - normal_p2 + radius_sum);
    float overlap1 = overlap * m1 / (m1 + m2);
    float overlap2 = overlap * m2 / (m1 + m2);

    float exit_time1 = overlap1 / normal_v1;
    float exit_time2 = overlap2 / normal_v2;

    float epsilon = 0.1;

    // Update
    v1 = {exit_vel1.x, exit_vel1.y};
    v2 = {exit_vel2.x, exit_vel2.y};

    p1 = {p1.x + v1.x * exit_time1 + epsilon, p1.y + v1.y * exit_time1 + epsilon};
    p1 = {p2.x + v2.x * exit_time2 + epsilon, p2.y + v2.y * exit_time2 + epsilon};
}

void quickAndDirtyTest(Position& p1, Position& p2, Velocity& v1,
                                  Velocity& v2, Radius r1, Radius r2) {
    if ( std::sqrt( (p1-p2)*(p1-p2) ) <= (r1.value+r2.value) ){
        rockCollision(p1, p2, v1,v2, r1, r2);
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
    for (auto i : it) {
        positions[i] += velocities[i] * it.delta_time();
    }
}

PhysicSystems::PhysicSystems(flecs::world &world) {
    world.module<PhysicSystems>();

    world.system<Position, Velocity>().with<Rock>().multi_threaded(true).iter(updateState);

    world.system().iter(clear);
    world.system<Position, Radius, Mountain>()
        .with<Rock>()
        .term_at(3)
        .singleton()
        .iter(debugRenderRocks);

    world.system<Position, Velocity, Radius, Mountain>()
        .term_at(4)
        .singleton()
        .iter(terrainCollision);

    for (int i = 0; i < 15; i++) {
        Position p{200.f + i * 20.f, 500.f};
        Velocity v{0., 0.};
        makeRock(world, p, v, 10.f);
    }
}
