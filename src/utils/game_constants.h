#ifndef SURVIVING_SARNTAL_GAME_CONSTANTS_H
#define SURVIVING_SARNTAL_GAME_CONSTANTS_H

typedef float float_type;

namespace graphics {
constexpr int SCREEN_WIDTH = 1600;
constexpr int SCREEN_HEIGHT = 900;
} // namespace graphics

/**
 * Player size
 */
constexpr float_type HIKER_HEIGHT = 50.f;
constexpr float_type HIKER_WIDTH = 15.f;
constexpr float_type DUCKED_HIKER_HEIGHT = 25.f;
constexpr float_type DUCKED_HIKER_WIDTH = 10.f;

/**
 * Player movement speed constants.
 */
constexpr float_type DUCK_SPEED_FACTOR = .5f;
constexpr float_type NORMAL_SPEED = 500.f;
constexpr float_type JUMP_VELOCITY_CONSTANT = 500.f;
constexpr float_type AIR_MOVEMENT_SPEED_FACTOR = .6f;
// Movement on a slope TODO these need to be fine-tuned!!!
// Speed constants
constexpr float_type MIN_SPEED_NEG_SLOPE = .6f;
constexpr float_type MAX_SPEED_NEG_SLOPE = 1.5f;
constexpr float_type MIN_SPEED_POS_SCOPE = .5f;
// Slope constants
constexpr float_type SLOWEST_NEG_SLOPE = -1.f;
constexpr float_type FASTEST_NEG_SCOPE = -.5f;
constexpr float_type SLOWEST_POS_SCOPE = 2.f;

/**
 * Player items
 */
constexpr float_type HIKER_ITEM_COLLECTION_RANGE = 200.f;

constexpr int INVENTORY_SLOTS = 3;
constexpr int KAISERSCHMARRN_HEALTH_RESTORATION = 30;
constexpr int COIN_SCORE = 500;

constexpr float_type ITEM_BASE_HEIGHT =
    40.; // minimal height at which items are placed above the ground
constexpr float_type ITEM_MAX_HEIGHT =
    200.; // max height above the terrain where items are spawned
constexpr float_type ITEMS_PER_SECOND = .05;

/**
 * Rock sizes
 */
constexpr float_type MIN_ROCK_SIZE = 10.f;
constexpr float_type MAX_ROCK_SIZE = 25.f;

/**
 * Rock speeds
 */
constexpr float_type VELOCITY_CAP = 600.;
constexpr float_type GAMMA = 0.5f;
constexpr float_type MAX_ANGULAR_VELOCITY = 1000.f;

/**
 * Rock spawn times
 */
constexpr float_type ROCK_TIME_PERIOD_DIFFICULT = 1;
constexpr float_type ROCK_TIME_PERIOD_MEDIUM = 5.;
constexpr float_type ROCK_TIME_PERIOD_EASY = 2;

enum RockSpawnPhase {
    veryBeginning,
    irregularRocks,
    regularRocks,
    rockBatches,
    explosiveBatches
};

struct RockSpawnParameters {
    int num_rocks_to_spawn;
    float time_between_rockspawns;
};

/**
 * Barriers
 */
constexpr float_type KILL_BAR_VELOCITY = 100.;
constexpr float_type ROCK_KILL_BAR_OFFSET = -(MAX_ROCK_SIZE + 50.f);
constexpr float_type PLAYER_RIGHT_BARRIER_OFFSET =
    (graphics::SCREEN_WIDTH * 0.8);

/**
 * Physical constants
 */
constexpr float_type GRAVITATIONAL_CONSTANT = -1000.f;
constexpr float_type EPSILON = 1e-3f;

extern bool kinect_mode;

constexpr int HIKER_MAX_HEALTH = 100;

#endif // SURVIVING_SARNTAL_GAME_CONSTANTS_H
