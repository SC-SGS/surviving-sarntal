#pragma once
#include "../utils/game_constants.h"

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

struct Health {
    int hp;
};

struct KillBar {
    float_type x;
};
