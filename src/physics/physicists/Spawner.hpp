//
// Created by felix-roehr on 5/27/24.
//

#ifndef SURVIVING_SARNTAL_SPAWNER_HPP
#define SURVIVING_SARNTAL_SPAWNER_HPP

#include "../../entities/World.h"
#include "../../spawner/ItemSpawner.h"
#include "../../spawner/RockSpawner.h"
#include <future>

class Spawner {

  public:
    Spawner(Terrain &terrain,
            RockSpawner &rockSpawner,
            ItemSpawner &itemSpawner,
            World &world,
            GameConstants &gameConstants);
    ~Spawner() = default;
    /**
     * @brief Spawns world entities
     *
     * Items and rocks are spawned.
     * The biome generation thread is started and joined when the hiker approaches the right border of the current
     * biome.
     */
    void spawn();

    void reset();

  private:
    RockSpawner &rockSpawner;
    Terrain &terrain;
    ItemSpawner &itemSpawner;
    World &world;
    GameConstants &gameConstants;

    bool generating = false;
    std::future<std::shared_ptr<Biome>> generatingThread;
};

#endif // SURVIVING_SARNTAL_SPAWNER_HPP
