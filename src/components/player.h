#pragma once

struct Player {};


struct PlayerMovement {
    enum MovementState { MOVING, JUMPING, IN_AIR, LANDING, IDLE };

    MovementState current_state;

};


struct KillBar {
    float x;
};
