//
// Created by Anietta Weckauff on 07.05.24.
//

#include "../RenderedEntity.h"
#include "../World.h"
#include <utility>

int RenderedEntity::idCounter = 0;

RenderedEntity::RenderedEntity(const Vector &position) : position(position) { id = idCounter++; }

void RenderedEntity::setAnimationInformation(const AnimationInformation animationInformation) {
    animation = animationInformation;
}

void RenderedEntity::setPosition(const Vector &position) { this->position = position; }
const Vector &RenderedEntity::getPosition() const { return position; }
