//
// Created by felix-roehr on 5/27/24.
//

#include "../Spawner.hpp"
#include "../../../spawner/RockSpawner.h"

#include <iostream>
#include <mutex>

Spawner::Spawner(Mountain &mountain, RockSpawner &rockSpawner, ItemSpawner &itemSpawner, World &world,
                 GameConstants gameConstants)
    : mountain(mountain), rockSpawner(rockSpawner), itemSpawner(itemSpawner), world(world),
      gameConstants(gameConstants) {}

void Spawner::spawn() const {
    if (this->world.getMaxX() >
        this->mountain.getRightBorder() - static_cast<floatType>(gameConstants.mountainConstants.chunkBufferRight) *
                                              gameConstants.mountainConstants.chunkWidth) {
        this->mountain.generateNewChunk();
    }
    this->rockSpawner.spawnRocks();
    this->itemSpawner.spawnItems();
}