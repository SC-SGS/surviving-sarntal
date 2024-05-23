//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "Monster.h"

float_type Monster::getXPosition() const { return position.x; }

void Monster::setXPosition(float_type x) { position.x = x; }

void Monster::updateMonsterPosition() {
    // setXPosition(getXPosition() + KILL_BAR_VELOCITY);
}
