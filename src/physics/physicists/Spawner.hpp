//
// Created by felix-roehr on 5/27/24.
//

#ifndef SURVIVING_SARNTAL_SPAWNER_HPP
#define SURVIVING_SARNTAL_SPAWNER_HPP

#include "../../entities/World.h"
#include "../../spawner/ItemSpawner.h"
#include "../../spawner/MountainGenerator.h"
#include "../../spawner/RockSpawner.h"

class Spawner {

  public:
    Spawner(MountainGenerator &mountainGenerator, RockSpawner &rockSpawner, ItemSpawner &itemSpawner);
    ~Spawner() = default;
    void spawn() const;

  private:
    RockSpawner &rockSpawner;
    MountainGenerator &mountainGenerator;
    ItemSpawner &itemSpawner;

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
