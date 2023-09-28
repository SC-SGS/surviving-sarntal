#include "physics.h"
#include "../components/render_components.h"
#include "flecs.h"
#include <cmath>
#include <iostream>
#include <thread>

using namespace physics;
using namespace math;

physics::Vertex physics::getClosestVertex(Position p, Radius r, Mountain *m) {
    float_type x_min = p.x - r.value;
    float_type x_max = p.x + r.value;
    auto interval = Mountain::getRelevantMountainSection(x_min, x_max);

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
    // R =  (  0   -1  )
    //      (  1    0  )
    Vector n = {-d.y, d.x};
    if (n.y < 0) {
        n = n * -1.f;
    }
    float_type normalization = std::sqrt(n * n);
    return n / normalization;
}

bool physics::isCollided(Position p1, Position p2, Radius r1, Radius r2) {
    return (p1.distanceTo(p2) <= r1.value + r2.value);
}

void physics::terrainCollision(flecs::iter it, Position *positions,
                               Velocity *velocities, Radius *r, Mountain *m,
                               Rotation *rot) {
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

        auto m = r[i].value * r[i].value;
        Vector parallel_vector = {-normal_vector.y, normal_vector.x};
        auto velocity_parallel = velocities[i] * parallel_vector;
        rot->angular_velocity +=
            GAMMA * (velocity_parallel - rot->angular_velocity * r[i].value) /
            m;
        if (rot->angular_velocity >= MAX_ANGULAR_VELOCITY) {
            rot->angular_velocity = MAX_ANGULAR_VELOCITY;
        }
        if (rot->angular_velocity <= -MAX_ANGULAR_VELOCITY) {
            rot->angular_velocity = -MAX_ANGULAR_VELOCITY;
        }
        rot->angular_offset += it.delta_time() * rot->angular_velocity;

        if (it.entity(i).has<Exploding>()) {
            explodeRock(it.world(), it.entity(i), 1);
        }
    }
}

void physics::makeRock(const flecs::world &world, Position p, Velocity v,
                       float_type radius) {
    world.entity()
        .set<Position>(p)
        .set<Velocity>(v)
        .set<Radius>({radius})
        .set<Rotation>({0, 0})
        .add<Rock>()
        //.set<graphics::CircleShapeRenderComponent>({radius})
        .set([&](graphics::BillboardComponent &c) {
            c = {0};
            c.billUp = {0.0f, 0.0f, 1.0f};
            c.billPositionStatic = {-radius / 2, 0.0f, -radius / 2};
            c.resourceHandle =
                world.get_mut<graphics::Resources>()->textures.load(
                    "../assets/texture/stone.png");
            c.width = radius * 2.0f;
            c.height = radius * 2.0f;
        });
    ;
}

void physics::rockCollision(Position &p1, Position &p2, Velocity &v1,
                            Velocity &v2, const Radius R1, const Radius R2,
                            float_type dt, float_type &ang_vel1,
                            float_type &ang_offset1, float_type &ang_vel2,
                            float_type &ang_offset2) {
    float_type m1 = R1.value * R1.value;
    float_type m2 = R2.value * R2.value;

    Vector vel_diff_vector = v1 - v2;
    Vector pos_diff_vector = p1 - p2;
    float_type distance_sq = pos_diff_vector * pos_diff_vector;

    // Velocity Update
    float_type total_mass = m1 + m2;
    v1 -= pos_diff_vector * 2 * m2 * (vel_diff_vector * pos_diff_vector) /
          (distance_sq * total_mass + EPSILON);
    v2 += pos_diff_vector * 2 * m1 * (vel_diff_vector * pos_diff_vector) /
          (distance_sq * total_mass + EPSILON);

    Vector normal_vector = {p2.y - p1.y, p1.x - p2.x};
    float_type relative_surface_velocity =
        ang_vel2 * R2.value - ang_vel1 * R1.value;

    ang_vel1 += GAMMA * std::abs((normal_vector * v1)) *
                relative_surface_velocity / (2 * m1);
    ang_vel2 -= GAMMA * std::abs((normal_vector * v2)) *
                relative_surface_velocity / (2 * m2);
    if (ang_vel1 >= MAX_ANGULAR_VELOCITY) {
        ang_vel1 = MAX_ANGULAR_VELOCITY;
    }
    if (ang_vel1 <= -MAX_ANGULAR_VELOCITY) {
        ang_vel1 = -MAX_ANGULAR_VELOCITY;
    }
    if (ang_vel2 >= MAX_ANGULAR_VELOCITY) {
        ang_vel2 = MAX_ANGULAR_VELOCITY;
    }
    if (ang_vel2 <= -MAX_ANGULAR_VELOCITY) {
        ang_vel2 = -MAX_ANGULAR_VELOCITY;
    }
    ang_offset1 += dt * ang_vel1;
    ang_offset2 += dt * ang_vel2;
}

void physics::rockRockInteractions(flecs::iter it, Position *positions,
                                   Velocity *velocities, Radius *radius,
                                   Rotation *rot) {
    for (int i = 0; i < it.count(); i++) {
        for (int j = i + 1; j < it.count(); j++) {
            auto next_pos1 = positions[i] + velocities[i] * it.delta_time();
            auto next_pos2 = positions[j] + velocities[j] * it.delta_time();
            if (isCollided((Position)next_pos1, (Position)next_pos2, radius[i],
                           radius[j])) {
                rockCollision(positions[i], positions[j], velocities[i],
                              velocities[j], radius[i], radius[j],
                              it.delta_time(), rot[i].angular_velocity,
                              rot[i].angular_offset, rot[j].angular_velocity,
                              rot[j].angular_offset); // TODO: Optimization?

                if (it.entity(i).has<Exploding>()) {
                    std::cout << "eskalation" << std::endl;
                    explodeRock(it.world(), it.entity(i), 1);
                }
            }
        }
    }
}

void physics::updateRockState(flecs::iter it, Position *positions,
                              Velocity *velocities, Rotation *rot) {
    updateRockVelocity(it, velocities);
    updateRockPosition(it, positions, velocities, rot);
    checkRockInScope(it, positions);
}

void physics::updateRockVelocity(flecs::iter it, Velocity *velocities) {
    for (auto i : it) {
        velocities[i].y += GRAVITATIONAL_CONSTANT * it.delta_time();
        if (velocities[i].length() > VELOCITY_CAP) {
            velocities[i] = (Velocity)(velocities[i] * VELOCITY_CAP /
                                       velocities[i].length());
        }
    }
}

void physics::updateRockPosition(flecs::iter it, Position *positions,
                                 Velocity *velocities, Rotation *rot) {
    for (auto i : it) {
        positions[i] += velocities[i] * it.delta_time();
        rot[i].angular_offset += rot[i].angular_velocity * it.delta_time();
    }
}

void physics::explodeRock(const flecs::world &world, flecs::entity rock,
                          const int number_of_rocks) {
    auto position = *rock.get<Position>();
    auto velocity = *rock.get<Velocity>();
    auto radius = rock.get<Radius>()->value;
    rock.destruct();

    float_type new_r = radius / std::sqrt(number_of_rocks);
    float_type delta_angle = 2.0 * PI / number_of_rocks;

    Position delta_direction, p;
    Velocity v;
    for (int i = 0; i < number_of_rocks; i++) {
        delta_direction =
            Position{std::sin(delta_angle * i), std::cos(delta_angle * i)};
        p = (Position)(position + delta_direction * radius);
        v = (Velocity)(velocity + delta_direction * 10);
        makeRock(world, p, v, new_r);
    }
}

void physics::checkRockInScope(flecs::iter it, Position *positions) {
    auto rock_kill_bar =
        it.world().get_mut<KillBar>()->x + ROCK_KILL_BAR_OFFSET;
    for (auto i : it) {
        if (positions[i].x < rock_kill_bar) {
            it.entity(i).destruct();
        }
    }
}

void physics::updatePlayerState(flecs::iter it, Position *positions,
                                Velocity *velocities,
                                PlayerMovement *player_movements,
                                InputEntity *input_entities, Height *heights,
                                Width *widths) {

    updatePlayerVelocity(it, positions, velocities, player_movements,
                         input_entities, heights, widths);
    updatePlayerPosition(it, positions, velocities, player_movements);
}

void physics::updatePlayerVelocity(flecs::iter it, Position *positions,
                                   Velocity *velocities,
                                   PlayerMovement *player_movements,
                                   InputEntity *input_entities, Height *heights,
                                   Width *widths) {

    checkJumpEvent(it, velocities, player_movements, input_entities);
    checkDuckEvent(it, velocities, player_movements, input_entities, heights,
                   widths);
    checkXMovement(velocities, player_movements, input_entities);
    checkAerialState(it, velocities, player_movements, input_entities);
    checkDirection(velocities, player_movements, input_entities);
}

void physics::updatePlayerPosition(flecs::iter it, Position *positions,
                                   Velocity *velocities,
                                   PlayerMovement *player_movements) {
    float_type knockback = 0;
    if (it.entity(0).has<IsHit>()){
        float_type radius = it.entity(0).get<IsHit>()->radius_rock;
        float_type velocity_rock = it.entity(0).get<IsHit>()->velocity_rock;
        knockback = velocity_rock *  KNOCKBACKCONST * radius;
        std::cout << "hit: " << knockback << std::endl;
        int counter = it.entity(0).get_mut<IsHit>()->counting_variable;
        counter++;
        it.entity(0).set<IsHit>({radius, velocity_rock, counter});
        if (counter >= 50){
            it.entity(0).remove<IsHit>();
        }
    }

    float height = it.entity(0).get<Height>()->h;
    if (player_movements[0].current_state ==
        PlayerMovement::MovementState::IN_AIR) {
        positions[0].x += knockback +
            AIR_MOVEMENT_SPEED_FACTOR * velocities[0].x * it.delta_time();
        auto terrain_y = getYPosFromX(it.world(), positions[0].x, height);
        auto air_y = positions[0].y + velocities[0].y * it.delta_time();
        if (air_y > terrain_y) {
            positions[0].y = air_y;
        } else {
            positions[0].y = terrain_y;
            player_movements[0].current_state =
                PlayerMovement::MovementState::MOVING;
            player_movements[0].can_jump_again = true;
            it.entity(0).remove<graphics::BillboardComponent>();
            it.entity(0).set([&](graphics::AnimatedBillboardComponent &c) {
                c = {0};
                c.billUp = {0.0f, 0.0f, 1.0f};
                c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
                c.resourceHandle =
                    it.world().get_mut<graphics::Resources>()->textures.load(
                        "../assets/texture/player_walk.png");
                c.width = HIKER_WIDTH; // TODO?
                c.height = HIKER_HEIGHT;
                c.current_frame = 0;
                c.numFrames = 4;
            });
        }
    } else if (player_movements[0].current_direction !=
               PlayerMovement::Direction::NEUTRAL) {
        auto next_x_pos = velocities[0].x * it.delta_time() + positions[0].x;
        auto next_y_pos = getYPosFromX(it.world(), next_x_pos, height);
        Vector direction = {next_x_pos - positions[0].x,
                            next_y_pos - positions[0].y};
        float length = std::sqrt(std::pow(next_x_pos - positions[0].x, 2) +
                                 std::pow(next_y_pos - positions[0].y, 2));
        float slope = direction.y / direction.x;
        float speed_factor = getSpeedFactor(slope);
        positions[0].x = (it.delta_time() *
                          std::abs(velocities[0].x * speed_factor) / length) *
                             direction.x + knockback + positions[0].x;
        positions[0].y = getYPosFromX(it.world(), positions[0].x, height);
    } else {
        positions[0].x += knockback;
        positions[0].y = getYPosFromX(it.world(), positions[0].x, height);
    }
    if (positions[0].x >
        it.world().get<KillBar>()->x + PLAYER_RIGHT_BARRIER_OFFSET) {
        positions[0].x =
            it.world().get<KillBar>()->x + PLAYER_RIGHT_BARRIER_OFFSET;
    }
}

float physics::getSpeedFactor(float slope) {
    if (slope <= SLOWEST_NEG_SLOPE) {
        return MIN_SPEED_NEG_SLOPE;
    } else if (slope <= FASTEST_NEG_SCOPE) {
        return linearInterpolation(slope,
                                   {SLOWEST_NEG_SLOPE, MIN_SPEED_NEG_SLOPE},
                                   {FASTEST_NEG_SCOPE, MAX_SPEED_NEG_SLOPE});
    } else if (slope <= 0) {
        return linearInterpolation(
            slope, {FASTEST_NEG_SCOPE, MAX_SPEED_NEG_SLOPE}, {0, 1});
    } else if (slope <= SLOWEST_POS_SCOPE) {
        return linearInterpolation(slope, {0, 1},
                                   {SLOWEST_POS_SCOPE, MIN_SPEED_POS_SCOPE});
    } else {
        return MIN_SPEED_POS_SCOPE;
    }
}

void physics::checkJumpEvent(flecs::iter it, Velocity *velocities,
                             PlayerMovement *player_movements,
                             InputEntity *input_entities) {
    if (input_entities->getEvent(Event::JUMP)) {
        if (player_movements[0].current_state ==
            PlayerMovement::MovementState::DUCKED) {
            return;
        }
        if (player_movements[0].current_state !=
            PlayerMovement::MovementState::IN_AIR) {
            player_movements[0].last_jump = 0;
        }
        if (player_movements[0].last_jump < 1.5 &&
            player_movements[0].can_jump_again) {
            PlaySound(jump_sound);
            velocities[0].y = JUMP_VELOCITY_CONSTANT;
            if (player_movements[0].current_state ==
                PlayerMovement::MovementState::IN_AIR) {
                player_movements[0].can_jump_again = false;
            }
            player_movements[0].current_state =
                PlayerMovement::MovementState::IN_AIR;
            it.entity(0).remove<graphics::AnimatedBillboardComponent>();
            it.entity(0).set([&](graphics::BillboardComponent &c) {
                c = {0};
                c.billUp = {0.0f, 0.0f, 1.0f};
                c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
                c.resourceHandle =
                    it.world().get_mut<graphics::Resources>()->textures.load(
                        "../assets/texture/hiker_jump.png");
                c.width = HIKER_WIDTH; // TODO?
                c.height = HIKER_HEIGHT;
            });
        }
    }
}

void physics::checkDuckEvent(flecs::iter it, Velocity *velocities,
                             PlayerMovement *player_movements,
                             InputEntity *input_entities, Height *heights,
                             Width *widths) {
    if (input_entities->getEvent(Event::DUCK) &&
        player_movements[0].current_state ==
            PlayerMovement::MovementState::MOVING) {
        player_movements[0].current_state =
            PlayerMovement::MovementState::DUCKED;
        heights[0].h = DUCKED_HIKER_HEIGHT;
        widths[0].w = DUCKED_HIKER_WIDTH;
        it.entity(0)
            .get_mut<graphics::RectangleShapeRenderComponent>()
            ->height = DUCKED_HIKER_HEIGHT;
        it.entity(0).get_mut<graphics::RectangleShapeRenderComponent>()->width =
            DUCKED_HIKER_WIDTH;
        it.entity(0).remove<graphics::AnimatedBillboardComponent>();
        it.entity(0).set([&](graphics::BillboardComponent &c) {
            c = {0};
            c.billUp = {0.0f, 0.0f, 1.0f};
            c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
            c.resourceHandle =
                it.world().get_mut<graphics::Resources>()->textures.load(
                    "../assets/texture/hiker_duck.png");
            c.width = DUCKED_HIKER_WIDTH; // TODO?
            c.height = DUCKED_HIKER_HEIGHT;
        });
    } else if (player_movements[0].current_state == PlayerMovement::DUCKED &&
               !input_entities->getEvent(Event::DUCK)) {
        player_movements[0].current_state =
            PlayerMovement::MovementState::MOVING;
        heights[0].h = HIKER_HEIGHT;
        widths[0].w = HIKER_WIDTH;
        it.entity(0)
            .get_mut<graphics::RectangleShapeRenderComponent>()
            ->height = HIKER_HEIGHT;
        it.entity(0).get_mut<graphics::RectangleShapeRenderComponent>()->width =
            HIKER_WIDTH;
        it.entity(0).remove<graphics::BillboardComponent>();
        it.entity(0).set([&](graphics::AnimatedBillboardComponent &c) {
            c = {0};
            c.billUp = {0.0f, 0.0f, 1.0f};
            c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
            c.resourceHandle =
                it.world().get_mut<graphics::Resources>()->textures.load(
                    "../assets/texture/player_walk.png");
            c.width = HIKER_WIDTH; // TODO?
            c.height = HIKER_HEIGHT;
            c.current_frame = 0;
            c.numFrames = 4;
        });
    }
}

void physics::checkXMovement(Velocity *velocities,
                             PlayerMovement *player_movements,
                             InputEntity *input_entities) {
    double x_factor = input_entities->getAxis(Axis::MOVEMENT_X);
    float_type speed = NORMAL_SPEED;
    if (player_movements[0].current_state ==
        PlayerMovement::MovementState::DUCKED) {
        speed *= DUCK_SPEED_FACTOR;
    }

    velocities[0].x = speed * (float_type)x_factor;
}

void physics::checkAerialState(flecs::iter it, Velocity *velocities,
                               PlayerMovement *player_movements,
                               InputEntity *input_entities) {
    if (player_movements[0].current_state ==
        PlayerMovement::MovementState::IN_AIR) {
        player_movements[0].last_jump += it.delta_time();
        velocities[0].y += GRAVITATIONAL_CONSTANT * it.delta_time();
    }
}

void physics::checkDirection(Velocity *velocities,
                             PlayerMovement *player_movements,
                             InputEntity *input_entities) {
    if (velocities[0].x < 0) {
        player_movements[0].current_direction = PlayerMovement::Direction::LEFT;
    } else if (velocities[0].x > 0) {
        player_movements[0].current_direction =
            PlayerMovement::Direction::RIGHT;
    } else {
        player_movements[0].current_direction =
            PlayerMovement::Direction::NEUTRAL;
    }
}

float physics::getYPosFromX(const flecs::world &world, float x, float offset) {
    auto mountain = world.get_mut<Mountain>();
    auto interval = Mountain::getRelevantMountainSection(x, x);
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

    return linearInterpolation(x, vertex_left, vertex_right) + offset;
}

void physics::checkPlayerIsHit(flecs::iter rock_it, Position *rock_positions,
                               Radius *radii, Velocity *rock_velocities) {
    rock_it.world()
        .filter_builder<Width, Height, Position, Health>()
        .with<Player>()
        .build()
        .iter([&](flecs::iter player_it, Width *widths, Height *heights,
                  Position *player_positions, Health *healths) {
            auto w = widths[0].w;
            auto h = heights[0].h;
            auto player_position = player_positions[0];
            bool is_hit = false;
            for (auto i : rock_it) {
                auto rock_position = rock_positions[i];
                auto x_centre_distance =
                    std::abs(player_position.x - rock_position.x);
                auto y_centre_distance =
                    std::abs(player_position.y - rock_position.y);
                auto r = radii[i].value;

                if (x_centre_distance > w / 2 + r ||
                    y_centre_distance > h / 2 + r) {
                } else if (x_centre_distance <= w / 2 ||
                           y_centre_distance <= h / 2) {
                    is_hit = true;
                } else {

                    auto corner_distance_sq =
                        std::pow(x_centre_distance - w / 2, 2) +
                        std::pow(y_centre_distance - h / 2, 2);

                    if (corner_distance_sq <= std::pow(r, 2)) {
                        is_hit = true;
                    }
                }
                if (is_hit) {
                    PlaySound(duck_sound);

                    int rock_dmg =
                        std::abs(49 * (radii[i].value - MIN_ROCK_SIZE)) /
                            (MAX_ROCK_SIZE - MIN_ROCK_SIZE) +
                        1;
                    std::cout << rock_dmg << std::endl;
                    healths[0].hp -= rock_dmg;
                    rock_it.entity(i).destruct();
                    player_it.entity(0).set<IsHit>({radii[i].value, rock_velocities[i].x, 0});
                    is_hit = false;
                    if (healths[0].hp <= 0) {
                        // TODO end animation or sth.
                        std::cout << "Player unalive" << std::endl;
                        rock_it.world().get_mut<AppInfo>()->playerAlive = false;
                        player_it.entity(0).destruct();
                    }
                }
            }
        });
}

void physics::initSounds() {
    duck_sound = LoadSound("../assets/audio/duck.wav");
    dudum_sound = LoadSound("../assets/audio/dudum.wav");
    mepmep_sound = LoadSound("../assets/audio/mepmep.wav");
    error_sound = LoadSound("../assets/audio/error.mp3");
    shutdown_sound = LoadSound("../assets/audio/shutdown.wav");
    gameover_sound = LoadSound("../assets/audio/gameover.wav");
    jump_sound = LoadSound("../assets/audio/jump.wav");
    pickup_sound = LoadSound("../assets/audio/pickup.wav");
}

float math::linearInterpolation(float x, Position left, Position right) {
    return ((x - left.x) * right.y + (right.x - x) * left.y) /
           (right.x - left.x);
}

void playDeadSoundSystem(flecs::iter, AppInfo *app_info) {
    if (!app_info->playerAlive && !app_info->playedDeadSound) {
        PlaySound(gameover_sound);
        app_info->playedDeadSound = true;
    }
}

PhysicSystems::PhysicSystems(flecs::world &world) {
    world.module<PhysicSystems>();

    world.system<Position, Velocity, Rotation>()
        .with<Rock>()
        .multi_threaded(true)
        .iter(updateRockState);

    world.system<Position, Velocity, Radius, Rotation>().with<Rock>().iter(
        rockRockInteractions);

    world.system<Position, Radius, Velocity>().with<Rock>().iter(checkPlayerIsHit);

    world.system<Position, Velocity, Radius, Mountain, Rotation>()
        .term_at(4)
        .singleton()
        .iter(terrainCollision);

    world
        .system<Position, Velocity, PlayerMovement, InputEntity, Height,
                Width>()
        .with<Player>()
        .iter(updatePlayerState);

    world.system<AppInfo>().term_at(1).singleton().iter(playDeadSoundSystem);

    // for (int i = 0; i < 20; i++) {
    //     Position p{300.f + 200.f, 25.f * (float)i};
    //     Velocity v{0., 0.};
    //     makeRock(world, p, v, 10.f);
    // }
}
