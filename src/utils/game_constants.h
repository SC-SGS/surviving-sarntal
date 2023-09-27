#ifndef SURVIVING_SARNTAL_GAME_CONSTANTS_H
#define SURVIVING_SARNTAL_GAME_CONSTANTS_H

typedef float float_type;

constexpr float_type KILL_BAR_VELOCITY = 100.;

constexpr float_type ROCK_TIME_PERIOD_DIFFICULT = 1;
constexpr float_type ROCK_TIME_PERIOD_MEDIUM = 0.5;
constexpr float_type ROCK_TIME_PERIOD_EASY = 2;

constexpr float_type DUCK_SPEED_FACTOR = .5f;
constexpr float_type NORMAL_SPEED = 500.f;
constexpr float_type JUMP_VELOCITY_CONSTANT = 500.f;
constexpr float_type HIKER_HEIGHT = 50.f;
constexpr float_type HIKER_WIDTH = 15.f;
constexpr float_type HIKER_ITEM_COLLECTION_RANGE = 10.f;
constexpr float_type GRAVITATIONAL_CONSTANT = -1000.f;
constexpr float_type EPSILON = 1e-3f;
constexpr float_type MIN_ROCK_SIZE = 10.f;
constexpr float_type MAX_ROCK_SIZE = 25.f;
constexpr float_type AIR_MOVEMENT_SPEED_FACTOR = .6f;

constexpr float_type ROCK_KILL_BAR_OFFSET = -(MAX_ROCK_SIZE + 50.f);
constexpr float_type PLAYER_RIGHT_BARRIER_OFFSET = 500.f;

#endif // SURVIVING_SARNTAL_GAME_CONSTANTS_H
