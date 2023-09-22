#pragma once

struct Player {};

struct Height {
    float h;
};

struct Width {
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
