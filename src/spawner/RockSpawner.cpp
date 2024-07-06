//
// Created by Anietta Weckauff on 13.05.24.
//

#include "RockSpawner.h"
#include <iostream>
#include <mutex>
#include <random>

RockSpawner::RockSpawner(World &world) : world(world){};

void RockSpawner::spawnRocks() {
    if (!shouldSpawnRocks()) {
        return;
    }

    const int numRocksToSpawn = computeNumRocksToSpawn();
    for (int i = 0; i < numRocksToSpawn; i++) {
        this->spawnRock(i);
    }
}

// NOLINTBEGIN
void RockSpawner::spawnRock(const size_t idxRock) {
    const std::vector<Vector> offsetsAdditionalRocks = getOffsetsAdditionalRocks();
    Vector spawnBasePos = getRandSpawnPos();
    const floatType rad =
        static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) * (MAX_ROCK_SIZE - MIN_ROCK_SIZE) + MIN_ROCK_SIZE;

    Vector velocity = {-MIN_SPAWN_VELOCITY - static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) *
                                                 (MAX_SPAWN_VELOCITY + MIN_SPAWN_VELOCITY),
                       0};
    floatType angularVelocity = MIN_SPAWN_VELOCITY + static_cast<float>(std::rand() / (1.0 * RAND_MAX)) *
                                                         (MAX_SPAWN_ROT_VELOCITY + MIN_SPAWN_ROT_VELOCITY);
    floatType angularOffset = 0.0f;

    Vector position = {spawnBasePos.x + offsetsAdditionalRocks[idxRock].x,
                       spawnBasePos.y + offsetsAdditionalRocks[idxRock].y};

    Rock newRock(position, velocity, angularVelocity, angularOffset, rad);

    this->world.addRock(newRock);
    spdlog::debug("Rock spawned at position (x: {0}, y: {1})", position.x, position.y);
}

bool RockSpawner::shouldSpawnRocks() {
    auto gameTime = GetTime();
    floatType timeBetweenRockSpawns = this->rockSpawnTimeFromPhase();
    bool shouldSpawn = gameTime > this->lastSpawnTime + timeBetweenRockSpawns;
    if (shouldSpawn) {
        this->lastSpawnTime = static_cast<floatType>(gameTime);
    }
    return shouldSpawn;
}

int RockSpawner::computeNumRocksToSpawn() {
    if (rockSpawnPhase == ROCK_BATCHES || rockSpawnPhase == EXPLOSIVE_BATCHES) {
        numberOfRocksSinceLastBatch = numberOfRocksSinceLastBatch++ % 3;
        if (numberOfRocksSinceLastBatch == 0) {
            return 3;
        }
    }
    return 1;
}

floatType RockSpawner::rockSpawnTimeFromPhase() {
    this->rockSpawnPhase = determineRockSpawnPhase();
    if (rockSpawnPhase == VERY_BEGINNING) {
        return 10.;
    } else if (rockSpawnPhase == IRREGULAR_ROCKS) {
        return 5.;
    } else if (rockSpawnPhase == REGULAR_ROCKS) {
        return 3.;
    } else { // rockSpawnPhase == ROCK_BATCHES
        return 4.;
    }
}

RockSpawnPhase RockSpawner::determineRockSpawnPhase() {
    auto gameTime = GetTime();
    if (gameTime < 5.) {
        return VERY_BEGINNING;
    } else if (gameTime < 20.) {
        return IRREGULAR_ROCKS;
    } else if (gameTime < 40.) {
        return REGULAR_ROCKS;
    } else if (gameTime < 80.) {
        return ROCK_BATCHES;
    } else {
        return EXPLOSIVE_BATCHES;
    }
}

std::vector<Vector> RockSpawner::getOffsetsAdditionalRocks() {
    return {{0., 0.}, {MAX_ROCK_SIZE + 5., MAX_ROCK_SIZE * 2 + 10.}, {-MAX_ROCK_SIZE - 5., MAX_ROCK_SIZE * 2 + 10.}};
}

Vector RockSpawner::getRandSpawnPos() const {
    // const auto spawnIndex = this->world.getMountain().getLatestChunk().startIndex;
    // const auto spawnXPos = this->world.getMountain().getVertex(spawnIndex).x;
    const auto spawnXPos = 2000.f + this->world.getMonster().getXPosition();
    const auto randYOffset =
        static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) * (400 - 300) + 300; // TODO these should be constants
    const auto spawnYPos = this->world.getMountain().getYPosFromX(spawnXPos) + randYOffset;
    return Vector{spawnXPos, spawnYPos};
}
// NOLINTEND
