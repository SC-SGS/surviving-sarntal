//
// Created by Anietta Weckauff on 07.05.24.
//

#include "../RenderedEntity.h"

#include "../../output/graphics/render_information/RenderInformation.h"
#include <utility>

RenderedEntity::RenderedEntity(Vector position) { this->position = position; }

RenderInformation RenderedEntity::getRenderInformation() { return {Vector2(position), 0, 0, {0, 0}, ""}; }

void RenderedEntity::setPosition(Vector &position) { this->position = position; }
Vector RenderedEntity::getPosition() { return position; }
