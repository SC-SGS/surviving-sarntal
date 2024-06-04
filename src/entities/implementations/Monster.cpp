//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Monster.h"
#include "../World.h"

#include <iostream>
#include <mutex>

Monster::Monster() : RenderedEntity({0.1 * graphics::SCREEN_WIDTH, 0}) {
    animation = {4, 0, 0.5, 0};
    std::cout << "Monster constructed" << std::endl;
}

Monster::~Monster() { std::cout << "Monster destructed" << std::endl; }

Monster::Monster(Vector position) : RenderedEntity(position) {}

RenderInformation Monster::getRenderInformation() {
    return RenderInformation{Vector2(position), {0, HIKER_HEIGHT / 2}, HIKER_WIDTH * 3, HIKER_HEIGHT, {0, 0}, "monster",
                             animation};
}

floatType Monster::getXPosition() const { return position.x; }

void Monster::setXPosition(floatType positionX) { position.x = positionX; }

// TODO needs delta time of the game (one time step) as an input to calculate the updated position
void Monster::updateMonsterPosition() {}
