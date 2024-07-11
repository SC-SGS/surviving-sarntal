//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_MONSTER_H
#define SURVIVING_SARNTAL_MONSTER_H

#include "../output/graphics/renderInformation/RenderInformation.h"
#include "../utilities/GameConstants.hpp"
#include "../utilities/Singleton.hpp"
#include "RenderedEntity.h"

#include <memory>

/**
 * This class represents the monster/ kill bar in the game. The kill bar moves
 * constantly. The player is killed as soon as he is reached by the kill bar.
 */
class Monster : public RenderedEntity {

  public:
    Monster();
    ~Monster() = default;
    Monster(const Monster &) = delete;

    explicit Monster(Vector position);

    RenderInformation getRenderInformation() const override;

    floatType getXPosition() const;
    void setXPosition(floatType positionX);
};

#endif // SURVIVING_SARNTAL_MONSTER_H
