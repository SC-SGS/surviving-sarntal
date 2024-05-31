//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Monster.h"
Monster::Monster(Vector position) : RenderedEntity(position) {}

float_type Monster::getXPosition() const { return position.x; }

void Monster::setXPosition(float_type positionX) { position.x = positionX; }

// TODO needs delta time of the game (one time step) as an input to calculate the updated position
void Monster::updateMonsterPosition() {}
