//
// Created by bjoern on 5/21/24.
//

#ifndef POSITIONER_H
#define POSITIONER_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"

class Positioner {

  public:
    explicit Positioner(World &world);
    ~Positioner() = default;
    void updatePositions();

    void setDeltaT(floatType deltaT);

  private:
    World &world;
    floatType deltaT;

    /**
     * Use movement equations to update rock positions.
     */
    void updateRockPositions() const;

    /**
     * Use movement equations to update rock positions.
     */
    void updateHikerPosition() const;

    /**
     * Moves the monster forward so that it always aligns with the left screen border.
     */
    void updateMonsterPosition();

    /**
     * Moves the left and right world border at each step to match the currently displayed part of the world.
     */
    void updateWorldBorderPosition() const;

    /**
     * Determines the degree to which the hiker speed is increased or decreased depending on the slope of the mountain
     * at his position.
     *
     * TODO this should be a smoother function
     *
     * @param slope
     * @return the speed factor
     */
    static floatType getSpeedFactor(floatType slope);
};

#endif // POSITIONER_H
