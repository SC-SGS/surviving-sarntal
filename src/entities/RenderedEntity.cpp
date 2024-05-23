//
// Created by Anietta Weckauff on 07.05.24.
//

#include "RenderedEntity.h"

RenderInformation RenderedEntity::getRenderInformation() { return renderInformation; }

void RenderedEntity::setPosition(Vector &newPosition) { this->position = newPosition; }

Vector RenderedEntity::getPosition() { return position; }

RenderedEntity::RenderedEntity(Vector position) { this->position = position; }
