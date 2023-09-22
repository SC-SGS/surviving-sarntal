#pragma once

constexpr float duckSpeedFactor = .5;
constexpr float normalSpeed = 500;
constexpr float JUMP_VELOCITY_CONSTANT = 10000;

struct Player {};


struct PlayerMovement {
    enum MovementState { MOVING, DUCKED, JUMPING, IN_AIR, LANDING, IDLE };

    MovementState current_state;
    float last_jump;

};


struct KillBar {
    float x;
};
