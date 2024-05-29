//
// Created by Anietta Weckauff on 04.05.24.
//

#include "../HikerMovement.h"
#include <string>

HikerMovement::HikerMovement() {
    currentState = MOVING;
    currentDirection = NEUTRAL;
    canJumpAgain = true;
    lastJump = 0;
}

HikerMovement::MovementState HikerMovement::getState() const { return currentState; }

// TODO where do we need this?
std::string HikerMovement::getStateString() const {
    switch (currentState) { // TODO take from config see issue #30 (you sure?)
    case MOVING:
        return "walk";
    case CROUCHED:
        return "crouch";
    case IN_AIR:
        return "jump";
    }
    return "UNKNOWN";
}

void HikerMovement::setState(const MovementState state) { this->currentState = state; }

HikerMovement::Direction HikerMovement::getDirection() const { return currentDirection; }

void HikerMovement::setDirection(const Direction direction) { this->currentDirection = direction; }

bool HikerMovement::getCanJumpAgain() const { return canJumpAgain; }

void HikerMovement::setCanJumpAgain(const bool canJump) { this->canJumpAgain = canJump; }

float HikerMovement::getLastJump() const { return lastJump; }

void HikerMovement::setLastJump(const float lastJump) { this->lastJump = lastJump; }
