#include "physics.h"
#include "../components/input.h"
#include "../components/mountain.h"
#include "../components/render_components.h"
#include "../components/vector.h"
#include "flecs.h"
#include "iostream"
#include "raylib.h"
#include <algorithm>
#include <cmath>

using namespace physics;

physics::Vertex physics::getClosestVertex(Position p, Radius r, Mountain *m) {
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

    return Vertex({closest_index, closest_distance});
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
    Vector normal_vector =
        pos_diff_vector * (1 / (pos_diff_vector * pos_diff_vector));

    // Wikipedia:
    Vector exit_vel1 = v1 - pos_diff_vector * (2 * m2 / (m1 + m2)) *
                                ((vel_diff_vector * pos_diff_vector) /
                                 (pos_diff_vector * pos_diff_vector));
    Vector exit_vel2 = v2 + pos_diff_vector * (2 * m1 / (m1 + m2)) *
                                ((vel_diff_vector * pos_diff_vector) /
                                 (pos_diff_vector * pos_diff_vector));

    float_type normal_v1 = exit_vel1 * normal_vector;
    float_type normal_v2 = exit_vel2 * normal_vector;
    float_type normal_p1 = p1 * normal_vector;
    float_type normal_p2 = p2 * normal_vector;

    float_type overlap = radius_sum - std::abs(normal_p1 - normal_p2);
    float_type overlap1 = overlap * m1 / (m1 + m2);
    float_type overlap2 = overlap * m2 / (m1 + m2);

    float_type exit_time1 = overlap1 / normal_v1;
    float_type exit_time2 = overlap2 / normal_v2;

    v1 = (Velocity)exit_vel1;
    v2 = (Velocity)exit_vel2;

    p1 += v1 * exit_time1 + EPSILON;
    p2 += v2 * exit_time2 + EPSILON;
}

void quickAndDirtyTest(Position &p1, Position &p2, Velocity &v1, Velocity &v2,
                       Radius r1, Radius r2) {
    if (std::sqrt((p1 - p2) * (p1 - p2)) <= (r1.value + r2.value)) {
        rockCollision(p1, p2, v1, v2, r1, r2);
    }
}

void physics::updateRockState(flecs::iter it, Position *positions,
                              Velocity *velocities) {
    updateRockVelocity(it, positions, velocities);
    updateRockPosition(it, positions, velocities);
}

void physics::updateRockVelocity(flecs::iter it, Position *positions,
                                 Velocity *velocities) {
    for (auto i : it) {
        velocities[i].y += GRAVITATIONAL_CONSTANT * it.delta_time();
    }
}

void physics::updateRockPosition(flecs::iter it, Position *positions,
                                 Velocity *velocities) {
    for (auto i : it) {
        positions[i] += velocities[i] * it.delta_time();
    }
}

PhysicSystems::PhysicSystems(flecs::world &world) {
    world.module<PhysicSystems>();

    world.system<Position, Velocity>().with<Rock>().multi_threaded(true).iter(
        updateRockState);

    world.system<Position, Velocity, Radius, Mountain>()
        .term_at(4)
        .singleton()
        .iter(terrainCollision);

    world.system<Position, Velocity, PlayerMovement>().with<Player>().iter(
        updatePlayerState);

    for (int i = 0; i < 10; i++) {
        Position p{200.f + i * 2.f, 500.f};
        Velocity v{0., 0.};
        makeRock(world, p, v, 10.f);
    }
}

void updatePlayerState(flecs::iter it, Position *positions,
                       Velocity *velocities, PlayerMovement *player_movements) {
    auto input = it.world().get<InputEntity>();

    // Update Velocities

    if (input->getEvent(Event::JUMP)) {
        float factor = 1;
        if (player_movements[0].current_state ==
            PlayerMovement::MovementState::DUCKED) {
            factor = duckSpeedFactor;
        }
        if (player_movements[0].current_state !=
            PlayerMovement::MovementState::IN_AIR) {
            player_movements[0].last_jump = 0;
        }
        if (player_movements[0].last_jump < 1.5) {
            velocities[0].y = JUMP_VELOCITY_CONSTANT * factor;
            player_movements[0].current_state =
                PlayerMovement::MovementState::IN_AIR;
        }
    }

    if (input->getEvent(Event::DUCK) &&
        player_movements[0].current_state !=
            PlayerMovement::MovementState::IN_AIR) {
        velocities[0].x *= duckSpeedFactor;
        player_movements[0].current_state =
            PlayerMovement::MovementState::DUCKED;
    }

    double x_factor = input->getAxis(Axis::MOVEMENT_X);
    if (player_movements[0].current_state !=
        PlayerMovement::MovementState::IN_AIR) {
        if (x_factor == 0) {
            player_movements[0].current_state =
                PlayerMovement::MovementState::IDLE;
        } else {
            player_movements[0].current_state =
                PlayerMovement::MovementState::MOVING;
        }
    }
    velocities[0].x = normalSpeed * x_factor;

    if (player_movements[0].current_state ==
        PlayerMovement::MovementState::IN_AIR) {
        player_movements[0].last_jump += it.delta_time();
        velocities[0].y += GRAVITATIONAL_CONSTANT * it.delta_time();
    }

    if (velocities[0].x < 0) {
        player_movements[0].current_direction = PlayerMovement::Direction::LEFT;
    } else if (velocities[0].x > 0) {
        player_movements[0].current_direction =
            PlayerMovement::Direction::RIGHT;
    } else {
        player_movements[0].current_direction =
            PlayerMovement::Direction::NEUTRAL;
    }

    // Update Positions

    positions[0].x += velocities[0].x * it.delta_time();
    auto terrain_y = getYPosFromX(it.world(), positions[0].x);
    if (player_movements[0].current_state ==
        PlayerMovement::MovementState::IN_AIR) {
        auto air_y = positions[0].y + velocities[0].y * it.delta_time();
        if (air_y > terrain_y) {
            positions[0].y = air_y;
        } else {
            positions[0].y = terrain_y;
            player_movements[0].current_state =
                PlayerMovement::MovementState::MOVING;
        }
    } else {
        positions[0].y = terrain_y;
    }
}

float getYPosFromX(const flecs::world &world, float x) {
    auto mountain = world.get_mut<Mountain>();
    auto interval = mountain->getRelevantMountainSection(x, x);
    std::size_t closest_indices[] = {interval.start_index, interval.end_index};
    auto closest_left_distance =
        std::abs(mountain->getVertex(interval.start_index).x - x);
    auto closest_right_distance =
        std::abs(mountain->getVertex(interval.end_index).x - x);

    for (auto j = interval.start_index; j < interval.end_index; j++) {
        auto mountain_vertex = mountain->getVertex(j);
        auto current_dist = mountain_vertex.x - x;

        if (current_dist < 0 &&
            std::abs(current_dist) < closest_left_distance) {
            closest_indices[0] = j;
            closest_left_distance = std::abs(current_dist);
        } else if (current_dist > 0 &&
                   std::abs(current_dist) < closest_right_distance) {
            closest_indices[1] = j;
            closest_right_distance = std::abs(current_dist);
        }
    }

    auto vertex_left = mountain->getVertex(closest_indices[0]);
    auto vertex_right = mountain->getVertex(closest_indices[1]);

    return ((x - vertex_left.x) * vertex_right.y +
            (vertex_right.x - x) * vertex_left.y) /
           (vertex_right.x - vertex_left.x);
}
