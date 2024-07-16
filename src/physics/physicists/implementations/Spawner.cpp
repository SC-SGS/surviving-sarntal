//
// Created by felix-roehr on 5/27/24.
//

#include "../Spawner.hpp"
#include "../../../spawner/RockSpawner.h"

#include <iostream>
#include <mutex>

Spawner::Spawner(Mountain &mountain, RockSpawner &rockSpawner, ItemSpawner &itemSpawner, World &world)
    : mountain(mountain), rockSpawner(rockSpawner), itemSpawner(itemSpawner), world(world) {}

void Spawner::spawn() const {
    if (this->world.getMaxX() > this->mountain.getRightBorder() - MOUNTAIN_CHUNK_BUFFER_RIGHT * MOUNTAIN_CHUNK_WIDTH) {
        this->mountain.generateNewChunk();
    }
    this->rockSpawner.spawnRocks();
    this->itemSpawner.spawnItems();
}