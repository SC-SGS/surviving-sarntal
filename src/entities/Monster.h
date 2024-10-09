//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_MONSTER_H
#define SURVIVING_SARNTAL_MONSTER_H

#include "../game/GameProperties.hpp"
#include "../output/graphics/renderInformation/RenderInformation.h"
#include "RenderedEntity.h"

/**
 * This class represents the monster/ kill bar in the game. The kill bar moves
 * constantly. The player is killed as soon as he is reached by the kill bar.
 */
class Monster final : public RenderedEntity {

  private:
    GameConstants &gameConstants;
    floatType velocity;

  public:
    floatType getVelocity() const;
    void setVelocity(floatType current_speed);

    explicit Monster(GameConstants &gameConstants);
    ~Monster() override = default;
    Monster(const Monster &) = delete;

    RenderInformation getRenderInformation() const override;

    floatType getXPosition() const;
    floatType getYPosition() const;
    void setXPosition(floatType positionX);

    const floatType monsterWidth;
    const floatType monsterHeight;
};

#endif // SURVIVING_SARNTAL_MONSTER_H
