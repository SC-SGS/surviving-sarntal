#pragma once

constexpr float duckSpeedFactor = .5;
constexpr float normalSpeed = 500;
constexpr float JUMP_VELOCITY_CONSTANT = 50;
constexpr float HIKER_HEIGHT = 25;
constexpr float HIKER_WIDTH = 10;

struct Player {};

struct Height{
    float h;
};

struct Width{
    float w;
};

class PlayerMovement {
  public:
    enum MovementState { MOVING, DUCKED, IN_AIR, IDLE };
    enum Direction { LEFT, RIGHT, NEUTRAL };

    MovementState current_state;
    Direction current_direction;
    bool can_jump_again;
    float last_jump;
};

struct KillBar {
    float x;
};
