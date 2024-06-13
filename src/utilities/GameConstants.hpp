#ifndef SURVIVING_SARNTAL_GAMECONSTANTS_HPP
#define SURVIVING_SARNTAL_GAMECONSTANTS_HPP

typedef float floatType;

namespace graphics {
constexpr int SCREEN_WIDTH = 1600;
constexpr int SCREEN_HEIGHT = 900;
}; // namespace graphics

constexpr floatType PHYSICS_DELTA_T = 0.01;

constexpr floatType PLAYER_SPAWN_OFFSET = 500;

/**
 * Player size
 */
constexpr floatType HIKER_HEIGHT = 90.6f;
constexpr floatType HIKER_WIDTH = 60.f; // 41.1f
constexpr floatType DUCKED_HIKER_HEIGHT = 65.9f;
constexpr floatType DUCKED_HIKER_WIDTH = 70.f; // 47.4f;

/**
 * Player movement speed constants.
 */
constexpr floatType DUCK_SPEED_FACTOR = .35f;
constexpr floatType NORMAL_SPEED = 200.f;
constexpr floatType JUMP_VELOCITY_CONSTANT = -750.f;
constexpr floatType AIR_MOVEMENT_SPEED_FACTOR = .6f;
constexpr floatType KNOCKBACKCONST = 0.00005f;
// Movement on a slope TODO these need to be fine-tuned!!!
// Speed constants
constexpr floatType MIN_SPEED_NEG_SLOPE = .6f;
constexpr floatType MAX_SPEED_NEG_SLOPE = 1.5f;
constexpr floatType MIN_SPEED_POS_SCOPE = .5f;
// Slope constants
constexpr floatType SLOWEST_NEG_SLOPE = -1.f;
constexpr floatType FASTEST_NEG_SCOPE = -.5f;
constexpr floatType SLOWEST_POS_SCOPE = 2.f;

/**
 * Player items
 */
constexpr floatType HIKER_ITEM_COLLECTION_RANGE = 200.f;

constexpr int SLOTS_PER_INVENTORY = 3;
constexpr int ITEMS_PER_SLOT = 64;
constexpr int KAISERSCHMARRN_HEALTH_RESTORATION = 30;
constexpr int COIN_SCORE = 500;

constexpr floatType SPAWN_START_TIME = 5.0;

constexpr floatType ITEM_BASE_HEIGHT = 40.; // minimal height at which items are placed above the ground
constexpr floatType ITEM_MAX_HEIGHT = 200.; // max height above the terrain where items are spawned
constexpr floatType ITEMS_PER_SECOND = .05;
constexpr floatType INVENTORY_SLOT_SIZE = 80.f;

/**
 * Rock sizes
 */
constexpr floatType MIN_ROCK_SIZE = 25.f;
constexpr floatType MAX_ROCK_SIZE = 75.f;

/**
 * Rock speeds
 */
constexpr floatType VELOCITY_CAP = 1200.;
constexpr floatType GAMMA = 50.f;
constexpr floatType MAX_ANGULAR_VELOCITY = 1000.f;
constexpr floatType MIN_SPAWN_VELOCITY = 1.f;
constexpr floatType MAX_SPAWN_VELOCITY = 1000.f;
constexpr floatType MIN_SPAWN_ROT_VELOCITY = 0.1f;
constexpr floatType MAX_SPAWN_ROT_VELOCITY = 100.f;

/**
 * Rock spawn times
 */
constexpr floatType ROCK_TIME_PERIOD_DIFFICULT = 1;
constexpr floatType ROCK_TIME_PERIOD_MEDIUM = 5.;
constexpr floatType ROCK_TIME_PERIOD_EASY = 2;

struct RockSpawnParameters {
    int num_rocks_to_spawn;
    floatType time_between_rockspawns;
};

/**
 * Barriers
 */
constexpr floatType KILL_BAR_VELOCITY = 100.;
constexpr floatType ROCK_KILL_BAR_OFFSET = -(MAX_ROCK_SIZE + 50.f);
constexpr floatType PLAYER_RIGHT_BARRIER_OFFSET = (graphics::SCREEN_WIDTH * 0.8);
constexpr floatType RUMBLE_ZONE = 200.f;

/**
 * Physical constants
 */
constexpr floatType GRAVITATIONAL_CONSTANT = 1000.f;
constexpr floatType EPSILON = 1e-3f;
constexpr floatType ROCK_TERRAIN_DAMPING = 0.8f;

constexpr int HIKER_MAX_HEALTH = 100;
constexpr int HEALTH_BAR_WIDTH = 200;
constexpr int HEALTH_BAR_HEIGHT = 20;

constexpr int UI_MARGIN = 20;

#endif // SURVIVING_SARNTAL_GAMECONSTANTS_HPP
