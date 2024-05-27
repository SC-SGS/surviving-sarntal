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
std::string HikerMovement::getStateString() const {
    switch (currentState) { // TODO take from config see issue #30
    case MOVING:
        return "walk";
    case DUCKED:
        return "crouch";
    case IN_AIR:
        return "jump";
    }
    return "UNKNOWN";
}

void HikerMovement::setState(MovementState state) { currentState = state; }

HikerMovement::Direction HikerMovement::getDirection() const { return currentDirection; }

void HikerMovement::setDirection(Direction direction) { currentDirection = direction; }

bool HikerMovement::getCanJumpAgain() const { return canJumpAgain; }

void HikerMovement::setCanJumpAgain(bool canJump) { canJumpAgain = canJump; }

float HikerMovement::getLastJump() const { return lastJump; }

void HikerMovement::setLastJump(float lastJump) { lastJump = lastJump; }
