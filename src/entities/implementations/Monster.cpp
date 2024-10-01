//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Monster.h"
#include "../World.h"

#include <iostream>
#include <mutex>

Monster::Monster(HikerConstants &hikerConstants)
    : hikerConstants(hikerConstants),
      RenderedEntity(
          {static_cast<floatType>(0.1 * (static_cast<float>(GetScreenWidth()) / graphics::UNIT_TO_PIXEL_RATIO)), 0}) {
    animation = {10, 0, 0.2, 0};
}

RenderInformation Monster::getRenderInformation() const {
    return RenderInformation{Vector2(position), {0, 1}, monsterWidth, monsterHeight, -5, "monster", animation};
}

floatType Monster::getXPosition() const { return position.x; }

floatType Monster::getYPosition() const { return position.y; }

void Monster::setXPosition(floatType positionX) { position.x = positionX; }