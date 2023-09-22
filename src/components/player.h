#pragma once

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
