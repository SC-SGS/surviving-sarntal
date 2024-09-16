//
// Created by Anietta Weckauff on 13.05.24.
//

#include "RockSpawner.h"

#include "PolygonGenerator.h"

#include <iostream>
#include <mutex>
#include <random>

RockSpawner::RockSpawner(World &world, GameConstants gameConstants) : world(world), gameConstants(gameConstants){};

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
    auto const &rockConst = this->gameConstants.rockConstants;
    const floatType rad =
        static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) * (rockConst.maxRockSize - rockConst.minRockSize) +
        rockConst.minRockSize;

    Vector velocity = {-rockConst.minSpawnVelocity - static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) *
                                                         (rockConst.maxSpawnVelocity + rockConst.minSpawnVelocity),
                       0};
    floatType angularVelocity = rockConst.minSpawnRotationVelocity +
                                static_cast<float>(std::rand() / (1.0 * RAND_MAX)) *
                                    (rockConst.maxSpawnRotationVelocity - rockConst.minSpawnRotationVelocity);
    floatType angularOffset = 0.0f;

    Vector position = {spawnBasePos.x + offsetsAdditionalRocks[idxRock].x,
                       spawnBasePos.y + offsetsAdditionalRocks[idxRock].y};

    PolygonGenerator polyGen;
    DynamicPolygon poly = polyGen.generatePolygon(100, rockConst.maxRockSize, position, 1.0f);
    DynamicProperties dynamicProperties{position, 0, velocity, 1};
    Rock newRock(position, poly.getBodySpaceVertices(), poly.getTextureCoordinates(), poly.getMass(),
                 poly.getMomentOfInertia(), dynamicProperties);

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
    auto &rockConst = gameConstants.rockConstants;
    return {
        {0., 0.},
        {static_cast<floatType>(rockConst.maxRockSize + 5.), static_cast<floatType>(rockConst.maxRockSize * 2 + 10.)},
        {static_cast<floatType>(-rockConst.maxRockSize - 5.), static_cast<floatType>(rockConst.maxRockSize * 2 + 10.)}};
}

Vector RockSpawner::getRandSpawnPos() const {
    const auto spawnXPos = this->world.getMaxX() + gameConstants.rockConstants.spawnOffsetX;
    const auto randYOffset =
        static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) * (4 - 3) + 3; // TODO these should be constants
    const auto spawnYPos = this->world.getTerrain().getMaxHeight(spawnXPos) + randYOffset;
    return Vector{spawnXPos, spawnYPos};
}
// NOLINTEND
