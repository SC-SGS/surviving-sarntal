#ifndef SURVIVING_SARNTAL_GAME_CONSTANTS_H
#define SURVIVING_SARNTAL_GAME_CONSTANTS_H

typedef float float_type;

constexpr float_type DUCK_SPEED_FACTOR = .5f;
constexpr float_type NORMAL_SPEED = 500.f;
constexpr float_type JUMP_VELOCITY_CONSTANT = 500.f;
constexpr float_type HIKER_HEIGHT = 50.f;
constexpr float_type HIKER_WIDTH = 15.f;
constexpr float_type HIKER_ITEM_COLLECTION_RANGE = 10.f;
constexpr float_type GRAVITATIONAL_CONSTANT = -1000.f;
constexpr float_type EPSILON = 1e-3f;

bool kinect_mode = false;

#endif // SURVIVING_SARNTAL_GAME_CONSTANTS_H
