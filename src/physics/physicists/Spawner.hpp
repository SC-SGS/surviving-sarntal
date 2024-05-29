//
// Created by felix-roehr on 5/27/24.
//

#ifndef SURVIVING_SARNTAL_SPAWNER_HPP
#define SURVIVING_SARNTAL_SPAWNER_HPP

#include "../../entities/World.h"
class Spawner {

  public:
    explicit Spawner(World &world);
    void spawn() const;

  private:
    World &world;

    /**
     * Spawn rocks for this time step.
     */
    void spawnRocks() const;

    /**
     * Spawn new chunks of the mountain if necessary in this time step;
     */
    void spawnMountain() const;
};

#endif // SURVIVING_SARNTAL_SPAWNER_HPP
