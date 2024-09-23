//
// Created by felix-roehr on 5/27/24.
//

#include "../Spawner.hpp"
#include "../../../spawner/RockSpawner.h"

#include <iostream>
#include <mutex>

Spawner::Spawner(
    Terrain &terrain, RockSpawner &rockSpawner, ItemSpawner &itemSpawner, World &world, GameConstants &gameConstants)
    : rockSpawner(rockSpawner),
      terrain(terrain),
      itemSpawner(itemSpawner),
      world(world),
      gameConstants(gameConstants) {}

void Spawner::spawn() {
    if (this->world.getMaxX() > this->terrain.getRightBorder() - gameConstants.terrainConstants.bufferRight &&
        !this->generating) {
        this->generating = true;
        this->generatingThread = std::async(&Terrain::generateBiome, &this->terrain, BiomeType::MOUNTAIN,
                                            this->gameConstants.terrainConstants.biomeWidth);
        // this->terrain.generateBiome(BiomeType::MOUNTAIN, gameConstants.terrainConstants.biomeWidth);
    }
    if (this->world.getMaxX() > this->terrain.getRightBorder() - this->gameConstants.hikerConstants.hikerWidth) {
        assert(this->generating);
        std::shared_ptr<Biome> newBiome = this->generatingThread.get();
        this->terrain.addBiome(newBiome);
        this->generating = false;
    }
    this->rockSpawner.spawnRocks();
    this->itemSpawner.spawnItems();
}