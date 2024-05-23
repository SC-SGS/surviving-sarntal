//
// Created by Anietta Weckauff on 04.05.24.
//

#include "HikerMovement.h"
#include <string>

HikerMovement::HikerMovement() {
    current_state = MOVING;
    current_direction = NEUTRAL;
    can_jump_again = true;
    last_jump = 0;
}

HikerMovement::MovementState HikerMovement::getState() const { return current_state; }
std::string HikerMovement::getStateString() const {
    switch (current_state) { // TODO take from config see issue #30 
    case MOVING:
        return "walk";
    case DUCKED:
        return "crouch";
    case IN_AIR:
        return "jump";
    }
    return "UNKNOWN";
}

void HikerMovement::setState(MovementState state) { current_state = state; }

HikerMovement::Direction HikerMovement::getDirection() const { return current_direction; }

void HikerMovement::setDirection(Direction direction) { current_direction = direction; }

bool HikerMovement::canJumpAgain() const { return can_jump_again; }

void HikerMovement::setCanJumpAgain(bool canJump) { can_jump_again = canJump; }

float HikerMovement::getLastJump() const { return last_jump; }

void HikerMovement::setLastJump(float lastJump) { last_jump = lastJump; }
