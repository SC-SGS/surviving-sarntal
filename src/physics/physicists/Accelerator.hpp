//
// Created by bjoern on 5/21/24.
//

#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"

class Accelerator {

  public:
    void accelerate() const;
    void setDeltaT(floatType deltaT);
    explicit Accelerator(World &world, GameConstants &gameConstants);
    ~Accelerator() = default;

  private:
    GameConstants &gameConstants;
    World &world;

    /**
     * The constant length of a simulation time interval.
     */
    floatType deltaT;

    /**
     * Calculates the sum of forces acting on the center of mass of a rock at the current time.
     * Includes gravitational forces
     * TODO friction
     *
     * @param rock
     * @return
     */
    Vector calculateTranslationalForces(const Rock &rock) const;

    /**
     * Calculates the sum of torque acting on the center of mass of a rock at the current time.
     * Includes
     * TODO friction
     *
     * @param rock
     * @return
     */
    floatType calculateTorque(const Rock &rock) const;

    /**
     * Retrieves all rocks in the world and updates their velocities.
     */
    void updateMomentumOfRocks() const;

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
};

#endif // ACCELERATOR_H
