//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_MONSTER_H
#define SURVIVING_SARNTAL_MONSTER_H

#include "../game/GameProperties.hpp"
#include "../output/graphics/renderInformation/RenderInformation.h"
#include "../utilities/Singleton.hpp"
#include "RenderedEntity.h"

#include <memory>

/**
 * This class represents the monster/ kill bar in the game. The kill bar moves
 * constantly. The player is killed as soon as he is reached by the kill bar.
 */
class Monster : public RenderedEntity {

  private:
    HikerConstants &hikerConstants;

  public:
    explicit Monster(HikerConstants &hikerConstants);
    ~Monster() override = default;
    Monster(const Monster &) = delete;

    RenderInformation getRenderInformation() const override;

    floatType getXPosition() const;
    floatType getYPosition() const;
    void setXPosition(floatType positionX);
};

#endif // SURVIVING_SARNTAL_MONSTER_H
