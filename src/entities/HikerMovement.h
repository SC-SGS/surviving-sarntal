//
// Created by Anietta Weckauff on 04.05.24.
//

#ifndef SURVIVING_SARNTAL_HIKERMOVEMENT_H
#define SURVIVING_SARNTAL_HIKERMOVEMENT_H

#include <string>

/**
 * This is a helper class that stores information about the movement of a hiker.
 * It stores information about the movement state as well as the direction the
 * hiker is currently facing.
 */
class HikerMovement {
  public:
    enum MovementState { MOVING, CROUCHED, IN_AIR };
    enum Direction { LEFT, RIGHT, NEUTRAL };

    HikerMovement();
    HikerMovement(MovementState state, Direction direction);

    MovementState getState() const;
    std::string getStateString() const;
    void setState(MovementState state);
    Direction getDirection() const;
    void setDirection(Direction direction);
    bool getCanJumpAgain() const;
    void setCanJumpAgain(bool canJump);
    float getLastJump() const;
    void setLastJump(float lastJump);

  private:
    MovementState currentState;
    Direction currentDirection;
    bool canJumpAgain{};
    float lastJump{};
};

#endif // SURVIVING_SARNTAL_HIKERMOVEMENT_H
