//
// Created by felix-roehr on 5/27/24.
//

#include "../Spawner.hpp"
#include "../../../spawner/MountainGenerator.h"
#include "../../../spawner/RockSpawner.h"

#include <iostream>
#include <mutex>

Spawner::Spawner(MountainGenerator &mountainGenerator, RockSpawner &rockSpawner, ItemSpawner &itemSpawner)
    : mountainGenerator(mountainGenerator), rockSpawner(rockSpawner), itemSpawner(itemSpawner) {}

void Spawner::spawn() const {
    this->mountainGenerator.generateMountainChunk();
    this->rockSpawner.spawnRocks();
    this->itemSpawner.spawnItems();
}