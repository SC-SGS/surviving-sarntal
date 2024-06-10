//
// Created by bjoern on 5/21/24.
//

#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"

class Accelerator : public Singleton<Accelerator> {
    friend class Singleton<Accelerator>;

  public:
    void accelerate() const;
    void setDeltaT(floatType deltaT);

  private:
    World &world;

    /**
     * The constant length of a simulation time interval.
     */
    floatType deltaT;

    /**
     * Retrieves all rocks in the world and updates their velocities.
     */
    void updateRockVelocities() const;

    /**
     * Updates the velocity of the hiker.
     *
     * This does not include the velocity update caused by player input, but solely the effects of the world on the
     * hiker. Right now, it is just the application of the gravitational force on the hiker.
     * // TODO hiker velocity should depend on slope here, but is implemented in the positioner
     */
    void updateHikerVelocity() const;

    /**
     * Checks the velocity to determine the direction of the hiker.
     */
    void updateHikerDirection() const;

    Accelerator();
    ~Accelerator();
};

#endif // ACCELERATOR_H
