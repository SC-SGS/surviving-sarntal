//
// Created by bjoern on 5/21/24.
//

#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include "../../entities/World.h"

class Accelerator {

  public:
    /**
     * @brief Updates the velocities of all movable world entities.
     *
     * Currently, the hiker, the monster and rocks are accelerated.
     * The monster is accelerated in x direction by acceleration values defined in the game constants.
     * The rocs and the hiker (if he is in air) air accelerated in y-direction by a gravitational body force.
     *
     */
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
     * Calculates the sum of forces applied to the center of mass of a rock at the current time.
     * Includes gravitational forces
     *
     * @param rock
     * @return
     */
    Vector calculateTranslationalForces(const Rock &rock) const;

    /**
     * Calculates the sum of torque acting on the center of mass of a rock at the current time.
     * Includes
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
     */
    void updateHikerVelocity() const;

    /**
     * Updates the monster's velocity based on the horizontal distance.
     */
    void updateMonsterVelocity() const;
};

#endif // ACCELERATOR_H
