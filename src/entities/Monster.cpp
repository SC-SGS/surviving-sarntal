//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "Monster.h"
Monster::Monster(Vector position) : RenderedEntity(position) {}

float_type Monster::getXPosition() const { return position.x; }

void Monster::setXPosition(float_type x) { position.x = x; }

// TODO needs delta time of the game (one time step) as an input to calculate the updated position
void Monster::updateMonsterPosition() {}
