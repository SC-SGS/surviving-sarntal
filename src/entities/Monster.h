//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_MONSTER_H
#define SURVIVING_SARNTAL_MONSTER_H

#include "../utils/game_constants.h"
#include "RenderInformation.h"
#include "RenderedEntity.h"

/**
 * This class represents the monster/ kill bar in the game. The kill bar moves
 * constantly. The player is killed as soon as he is reached by the kill bar.
 */
class Monster : public RenderedEntity {
  public:
    Monster();

    float_type getXPosition() const;
    void setXPosition(float_type x);

    // what is the data type of delta time?
    void updateMonsterPosition();
};

#endif // SURVIVING_SARNTAL_MONSTER_H
