//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_MONSTER_H
#define SURVIVING_SARNTAL_MONSTER_H

#include "../output/graphics/render_information/RenderInformation.h"
#include "../utils/game_constants.h"
#include "RenderedEntity.h"

/**
 * This class represents the monster/ kill bar in the game. The kill bar moves
 * constantly. The player is killed as soon as he is reached by the kill bar.
 */
class Monster : public RenderedEntity {
  public:
    explicit Monster(Vector position);

    float_type getXPosition() const;
    void setXPosition(float_type positionX);

    // what is the data type of delta time?
    void updateMonsterPosition();
};

#endif // SURVIVING_SARNTAL_MONSTER_H
