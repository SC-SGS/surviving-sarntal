//
// Created by felix-roehr on 5/27/24.
//

#include "Spawner.hpp"
#include "../../factory/MountainGenerator.h"
#include "../../factory/RockSpawner.h"

Spawner::Spawner(World &world) : world(world) {}

void Spawner::spawn() const {
    spawnRocks();
    spawnMountain();
}

void Spawner::spawnRocks() const { RockSpawner::spawnRocks(&this->world, &RockSpawner::spawnData); }

void Spawner::spawnMountain() const {
    // TODO change this, so that generating and deleting a chunk can be split.
    // TODO: Also, the current state of the mountain generation is really confusing. Change that too.
    MountainGenerator::generateMountainChunk(&world);
}