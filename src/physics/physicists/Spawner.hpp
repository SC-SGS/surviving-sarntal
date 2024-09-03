//
// Created by felix-roehr on 5/27/24.
//

#ifndef SURVIVING_SARNTAL_SPAWNER_HPP
#define SURVIVING_SARNTAL_SPAWNER_HPP

#include "../../entities/World.h"
#include "../../spawner/ItemSpawner.h"
#include "../../spawner/RockSpawner.h"

class Spawner {

  public:
    Spawner(Terrain &terrain, RockSpawner &rockSpawner, ItemSpawner &itemSpawner, World &world,
            GameConstants gameConstants);
    ~Spawner() = default;
    void spawn() const;

  private:
    RockSpawner &rockSpawner;
    Terrain &terrain;
    ItemSpawner &itemSpawner;
    World &world;
    GameConstants gameConstants;

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
