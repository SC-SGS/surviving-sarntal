//
// Created by felix-roehr on 5/27/24.
//

#include "../Spawner.hpp"
#include "../../../spawner/RockSpawner.h"

#include <iostream>
#include <mutex>

Spawner::Spawner(Terrain &terrain, RockSpawner &rockSpawner, ItemSpawner &itemSpawner, World &world,
                 GameConstants gameConstants)
    : terrain(terrain), rockSpawner(rockSpawner), itemSpawner(itemSpawner), world(world), gameConstants(gameConstants) {
}

void Spawner::spawn() const {
    if (this->world.getMaxX() > this->terrain.getRightBorder() - gameConstants.terrainConstants.bufferRight) {
        this->terrain.generateBiome(BiomeType::MOUNTAIN, gameConstants.terrainConstants.biomeWidth);
    }
    this->rockSpawner.spawnRocks();
    this->itemSpawner.spawnItems();
}