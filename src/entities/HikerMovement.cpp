//
// Created by Anietta Weckauff on 04.05.24.
//

#include "HikerMovement.h"

HikerMovement::HikerMovement() {}

HikerMovement::MovementState HikerMovement::getState() const {
    return current_state;
}

void HikerMovement::setState(MovementState state) { current_state = state; }

HikerMovement::Direction HikerMovement::getDirection() const {
    return current_direction;
}

void HikerMovement::setDirection(Direction direction) {
    current_direction = direction;
}

bool HikerMovement::canJumpAgain() const { return can_jump_again; }

void HikerMovement::setCanJumpAgain(bool canJump) { can_jump_again = canJump; }

float HikerMovement::getLastJump() const { return last_jump; }

void HikerMovement::setLastJump(float lastJump) { last_jump = lastJump; }
