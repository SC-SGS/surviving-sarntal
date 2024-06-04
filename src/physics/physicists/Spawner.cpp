//
// Created by felix-roehr on 5/27/24.
//

#include "Spawner.hpp"
#include "../../factory/MountainGenerator.h"
#include "../../factory/RockSpawner.h"

#include <iostream>
#include <mutex>

Spawner::Spawner() { std::cout << "Spawner gets constructed" << std::endl; }

Spawner::~Spawner() { std::cout << "Spawner gets deconstructed" << std::endl; }

void Spawner::spawn() const {
    this->mountainGenerator.generateMountainChunk();
    this->rockSpawner.spawnRocks();
    this->itemSpawner.spawnItems();
}