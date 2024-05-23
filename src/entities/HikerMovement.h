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
    enum MovementState { MOVING, DUCKED, IN_AIR };
    enum Direction { LEFT, RIGHT, NEUTRAL };

    HikerMovement();

    MovementState getState() const;
    std::string getStateString() const;
    void setState(MovementState state);
    Direction getDirection() const;
    void setDirection(Direction direction);
    bool canJumpAgain() const;
    void setCanJumpAgain(bool canJump);
    float getLastJump() const;
    void setLastJump(float lastJump);

  private:
    MovementState current_state;
    Direction current_direction;
    bool can_jump_again{};
    float last_jump{};
};

#endif // SURVIVING_SARNTAL_HIKERMOVEMENT_H
