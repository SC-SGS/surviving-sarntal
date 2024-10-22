//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Monster.h"
#include "../World.h"

Monster::Monster(GameConstants &gameConstants)
    : RenderedEntity({0, 0}),
      gameConstants(gameConstants),
      velocity(gameConstants.barriersConstants.killBarBaseVelocity),
      monsterWidth(gameConstants.barriersConstants.monsterWidth),
      monsterHeight(gameConstants.barriersConstants.monsterHeight) {
    animation = {10, 0, 0.2, 0};
}

RenderInformation Monster::getRenderInformation() const {
    return RenderInformation{Vector2(position), {0, 1}, monsterWidth, monsterHeight, -5, "monster", animation};
}

floatType Monster::getXPosition() const { return position.x; }

floatType Monster::getYPosition() const { return position.y; }

void Monster::setXPosition(floatType positionX) { position.x = positionX; }

floatType Monster::getVelocity() const { return this->velocity; }
void Monster::setVelocity(const floatType current_speed) {
    if (current_speed > gameConstants.barriersConstants.killBarMaxVelocity) {
        this->velocity = gameConstants.barriersConstants.killBarMaxVelocity;
    } else {
        this->velocity = current_speed;
    }
}