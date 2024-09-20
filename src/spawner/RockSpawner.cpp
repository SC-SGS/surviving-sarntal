//
// Created by Anietta Weckauff on 13.05.24.
//

#include "RockSpawner.h"

#include "PolygonGenerator.h"

#include <iostream>
#include <mutex>
#include <random>
#include <utility>

RockSpawner::RockSpawner(World &world, GameConstants &gameConstants) : world(world), gameConstants(gameConstants){};

void RockSpawner::spawnRocks() {
    if (!shouldSpawnRocks()) {
        return;
    }

    const int numRocksToSpawn = computeNumRocksToSpawn();
    for (int i = 0; i < numRocksToSpawn; i++) {
        this->spawnRock(i);
    }
}

void RockSpawner::spawnRock(const size_t idxRock) {
    Vector velocity = this->getRandVelocity();
    Vector position = this->getRockSpawnPosition(idxRock);
    DynamicPolygon poly = this->getRandDynamicPolygon(position);
    DynamicProperties dynamicProperties{position, 0, velocity, 1};
    this->createRock(position, poly, dynamicProperties);
    spdlog::debug("Rock spawned at position (x: {0}, y: {1})", position.x, position.y);
}

Vector RockSpawner::getRockSpawnPosition(const size_t idxRock) {
    const std::vector<Vector> offsetsAdditionalRocks = getOffsetsAdditionalRocks();
    Vector spawnBasePos = getRandSpawnPos();
    return {spawnBasePos.x + offsetsAdditionalRocks[idxRock].x, spawnBasePos.y + offsetsAdditionalRocks[idxRock].y};
}

DynamicPolygon RockSpawner::getRandDynamicPolygon(const Vector position) {
    PolygonGenerator polyGen;
    return polyGen.generatePolygon(this->getRandPointNumber(), this->getRandRadius(), position, this->getRandDensity());
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
    if (rockSpawnDifficulty == HARD || rockSpawnDifficulty == I_WANT_MOMY) {
        numberOfRocksSinceLastBatch = numberOfRocksSinceLastBatch++ % 3;
        if (numberOfRocksSinceLastBatch == 0) {
            return 3;
        }
    }
    return 1;
}

floatType RockSpawner::rockSpawnTimeFromPhase() {
    std::vector<floatType> spawnTimeInterval = gameConstants.rockSpawnerConstants.rockSpawnTimeInterval;
    return spawnTimeInterval[this->determineRockSpawnDifficulty()];
}

RockSpawnDifficulty RockSpawner::determineRockSpawnDifficulty() const {
    floatType currentXCoordinate = this->world.getMaxX();
    std::vector<floatType> spawningPhase = gameConstants.rockSpawnerConstants.spawningPhase;
    for (int i = 0; i < spawningPhase.size(); i++) {
        if (currentXCoordinate < spawningPhase[i]) {
            return RockSpawnDifficulty(i);
        }
    }
    return RockSpawnDifficulty(spawningPhase.size() - 1);
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
    const auto randYOffset = randomGenerator->getRandomRealNumber(6, 8);

    // static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) * (4 - 3) + 3; // TODO these should be constants
    const auto spawnYPos = this->world.getTerrain().getMaxHeight(spawnXPos) + randYOffset;
    return Vector{spawnXPos, spawnYPos};
}

int RockSpawner::getRandPointNumber() const { return randomGenerator->getRandomNumber(50, 200); }

floatType RockSpawner::getRandDensity() const {
    auto &rockConst = gameConstants.rockConstants;
    return randomGenerator->getRandomRealNumber(rockConst.minRockDensity, rockConst.maxRockDensity);
}

Vector RockSpawner::getRandVelocity() const {
    floatType lowerBoundVelocity = this->determineLowerBoundVelocity();
    floatType upperBoundVelocity = this->gameConstants.rockConstants.maxSpawnVelocity;
    floatType xComponent = -1 * randomGenerator->getRandomRealNumber(lowerBoundVelocity, upperBoundVelocity);
    return {xComponent, 0};
}

floatType RockSpawner::determineLowerBoundVelocity() const {
    auto const &rockConst = this->gameConstants.rockConstants;
    floatType difficultyFactor = determineDifficultyFactor();
    return difficultyFactor * (rockConst.maxSpawnVelocity - rockConst.minSpawnVelocity) + rockConst.minSpawnVelocity;
}

floatType RockSpawner::determineDifficultyFactor() const {
    std::vector<floatType> velocityDifficultyFactor = gameConstants.rockSpawnerConstants.velocityDifficultyFactor;
    return velocityDifficultyFactor[this->determineRockSpawnDifficulty()];
}

void RockSpawner::createRock(Vector &position, DynamicPolygon &poly, DynamicProperties &dynamicProperties) {
    auto rock = Rock(position, poly, dynamicProperties, this->determineRockTypePhase());
    this->world.addRock(std::make_shared<Rock>(rock));
}

floatType RockSpawner::getRandRadius() const {
    floatType maxRockSize = gameConstants.rockConstants.maxRockSize;
    return randomGenerator->getRandomRealNumber(4 * maxRockSize / 5, maxRockSize);
}

RockType RockSpawner::determineRockTypePhase() {
    floatType currentXCoordinate = this->world.getMaxX();
    std::vector<floatType> rockTypePhase = gameConstants.rockSpawnerConstants.rockTypePhase;
    for (int i = 0; i < rockTypePhase.size(); i++) {
        if (currentXCoordinate < rockTypePhase[i]) {
            return RockType(i);
        }
    }
    return RockType(0);
}