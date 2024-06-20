//
// Created by Anietta Weckauff on 07.05.24.
//

#include "../RenderedEntity.h"
#include <utility>

RenderedEntity::RenderedEntity(Vector position) { this->position = position; }

RenderInformation RenderedEntity::getRenderInformation() { return {Vector2(position), {0, 0}, 0, 0, 0, "", animation}; }

void RenderedEntity::setAnimationInformation(const AnimationInformation animationInformation) {
    animation = animationInformation;
}

void RenderedEntity::setPosition(Vector &position) { this->position = position; }
const Vector &RenderedEntity::getPosition() const { return position; }
