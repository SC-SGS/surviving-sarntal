//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Monster.h"
#include "../World.h"

#include <iostream>
#include <mutex>

Monster::Monster() : RenderedEntity({0.1 * graphics::SCREEN_WIDTH, 0}) { animation = {10, 0, 0.2, 0}; }

Monster::Monster(Vector position) : RenderedEntity(position) {}

RenderInformation Monster::getRenderInformation() const {
    return RenderInformation{Vector2(position), {0, 100}, HIKER_WIDTH * 2.5, HIKER_HEIGHT * 2, -5,
                             "monster",         animation};
}

floatType Monster::getXPosition() const { return position.x; }

void Monster::setXPosition(floatType positionX) { position.x = positionX; }