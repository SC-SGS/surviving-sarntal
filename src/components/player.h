#pragma once

constexpr float duckSpeedFactor = .5;
constexpr float normalSpeed = 500;
constexpr float JUMP_VELOCITY_CONSTANT = 1000;

struct Player {};

class PlayerMovement {
  public:
    enum MovementState { MOVING, DUCKED, IN_AIR, IDLE };
    enum Direction { LEFT, RIGHT, NEUTRAL };

    MovementState current_state;
    Direction current_direction;

    float last_jump;
};

struct KillBar {
    float x;
};
