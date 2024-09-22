//
// Created by Anietta Weckauff on 07.05.24.
//

#include "../RenderedEntity.h"
#include "../World.h"
#include <utility>

int RenderedEntity::idCounter = 0;

RenderedEntity::RenderedEntity(Vector position) : position(position) { id = idCounter++; }

void RenderedEntity::setAnimationInformation(const AnimationInformation animationInformation) {
    animation = animationInformation;
}

void RenderedEntity::setPosition(Vector &newPosition) { this->position = newPosition; }
const Vector &RenderedEntity::getPosition() const { return position; }
