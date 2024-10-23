//
// Created by Anietta Weckauff on 13.05.24.
//

#include "RockSpawner.h"
#include "PolygonGenerator.h"

#include <iostream>
#include <mutex>
#include <random>
#include <utility>

RockSpawner::RockSpawner(World &world, DifficultyService &difficultyService, GameConstants &gameConstants)
    : world(world), difficultyService(difficultyService), gameConstants(gameConstants){};

void RockSpawner::spawnRocks() {
    if (shouldSpawnRocks()) {
        const int numRocksToSpawn = computeNumRocksToSpawn();
        for (int i = 0; i < numRocksToSpawn; i++) {
            this->spawnRock(i);
        }
        this->updateNextSpawnTime();
    }
}

void RockSpawner::spawnRock(const size_t idxRock) const {
    Vector position = this->getRockSpawnPosition(idxRock);

    const DynamicConvexPolygon poly = this->getRandDynamicPolygon(position);
    const Vector linearMomentum =
        this->getRandLinearMomentum() * (float)DifficultyService::getInstance().getCurrentDifficultyLevel();
    const floatType angularMomentum = this->getRandAngularMomentum();
    const DynamicProperties dynamicProperties{position, 0, 0, linearMomentum, angularMomentum};
    this->createRock(position, poly, dynamicProperties);
    spdlog::debug("Rock spawned at position (x: {0}, y: {1})", position.x, position.y);
}

Vector RockSpawner::getRandLinearMomentum() const {
    const floatType lowerBoundLinearMomentum = this->determineLowerBoundLinearMomentum();
    const floatType upperBoundLinearMomentum = this->gameConstants.rockSpawnerConstants.maxLinearMomentum;
    const floatType xComponent =
        -1 * randomGenerator->getRandomRealNumber(lowerBoundLinearMomentum, upperBoundLinearMomentum);
    return {xComponent, 0};
}

Vector RockSpawner::getRockSpawnPosition(const size_t idxRock) const {
    const std::vector<Vector> offsetsAdditionalRocks = getOffsetsAdditionalRocks();
    const Vector spawnBasePos = getRandSpawnPos();
    return {spawnBasePos.x + offsetsAdditionalRocks[idxRock].x, spawnBasePos.y + offsetsAdditionalRocks[idxRock].y};
}

DynamicConvexPolygon RockSpawner::getRandDynamicPolygon(const Vector &position) const {
    const PolygonGenerator polyGen;
    return polyGen.generatePolygon(this->getRandPointNumber(), this->getRandRadius(), position, this->getRandDensity());
}

bool RockSpawner::shouldSpawnRocks() const {
    const auto gameTime = DifficultyService::getInstance().getElapsedGamePlayTime();
    bool timeToSpawn = this->nextSpawnTime <= gameTime;
    bool spawningIsOn = this->gameConstants.rockConstants.spawnRocks;
    return timeToSpawn && spawningIsOn;
}

void RockSpawner::updateNextSpawnTime() {
    auto gameTime = static_cast<floatType>(DifficultyService::getInstance().getElapsedGamePlayTime());
    const floatType timeBetweenRockSpawns = this->rockSpawnTimeFromPhase();
    this->nextSpawnTime = gameTime + timeBetweenRockSpawns;
}

int RockSpawner::computeNumRocksToSpawn() const {
    int difficultyLevel = DifficultyService::getInstance().getCurrentDifficultyLevel();
    switch (difficultyLevel) {
    case 0:
    case 1:
    case 2:
        return 1;
    case 3:
    case 4:
        return 2;
    case 5:
        return 3;
    default:
        return 2;
    }
}

floatType RockSpawner::rockSpawnTimeFromPhase() const {
    const std::vector<floatType> spawnTimeInterval = gameConstants.rockSpawnerConstants.rockSpawnTimeInterval;
    int diffLevel = DifficultyService::getInstance().getCurrentDifficultyLevel();
    int spawnTime = std::min(diffLevel, (int)spawnTimeInterval.size() - 1);

    return spawnTimeInterval[spawnTime];
}

std::vector<Vector> RockSpawner::getOffsetsAdditionalRocks() const {
    const auto maxRockSize = gameConstants.rockConstants.maxRockSize;
    return {{0., 0.},
            {static_cast<floatType>(maxRockSize + 10.), static_cast<floatType>(maxRockSize * 2 + 10.)},
            {static_cast<floatType>(-maxRockSize + 5.), static_cast<floatType>(maxRockSize * 2 + 10.)}};
}

Vector RockSpawner::getRandSpawnPos() const {
    const auto spawnXPos = this->world.getMaxX() + gameConstants.rockConstants.spawnOffsetX;

    const auto &rockSpawnConst = gameConstants.rockSpawnerConstants;
    const auto randYOffset =
        randomGenerator->getRandomRealNumber(rockSpawnConst.minRandYSpawnOffset, rockSpawnConst.maxRandYSpawnOffset);
    const auto spawnYPos = this->world.getTerrain().getMaxHeight(spawnXPos) + randYOffset;
    return Vector{spawnXPos, spawnYPos};
}

int RockSpawner::getRandPointNumber() const {
    const auto &rockSpawnConst = gameConstants.rockSpawnerConstants;
    return randomGenerator->getRandomNumber(rockSpawnConst.minNumPointsForGeneration,
                                            rockSpawnConst.maxNumPointsForGeneration);
}

floatType RockSpawner::getRandDensity() const {
    const auto &rockConst = gameConstants.rockConstants;
    return randomGenerator->getRandomRealNumber(rockConst.minRockDensity, rockConst.maxRockDensity);
}

floatType RockSpawner::determineLowerBoundLinearMomentum() const {
    auto const &rockConst = this->gameConstants.rockSpawnerConstants;
    const floatType difficultyFactor = determineDifficultyFactor();
    return difficultyFactor * (rockConst.maxLinearMomentum - rockConst.minLinearMomentum) + rockConst.minLinearMomentum;
}

floatType RockSpawner::determineDifficultyFactor() const {
    const auto linearMomentumDifficultyFactor = gameConstants.rockSpawnerConstants.linearMomentumDifficultyFactor;
    const int diffLevel = DifficultyService::getInstance().getCurrentDifficultyLevel();
    const int diffFactor = std::min(diffLevel, (int)linearMomentumDifficultyFactor.size() - 1);
    return linearMomentumDifficultyFactor[diffFactor];
}

void RockSpawner::createRock(const Vector &position,
                             const DynamicConvexPolygon &poly,
                             const DynamicProperties &dynamicProperties) const {
    auto rock = Rock(position, poly, dynamicProperties, this->determineRockTypePhase());
    this->world.addRock(std::make_shared<Rock>(rock));
}

floatType RockSpawner::getRandRadius() const {
    const floatType maxRockSize = gameConstants.rockConstants.maxRockSize;
    return randomGenerator->getRandomRealNumber(4 * maxRockSize / 5, maxRockSize);
}

RockType RockSpawner::determineRockTypePhase() const {
    int diffLevel = DifficultyService::getInstance().getCurrentDifficultyLevel();
    switch (diffLevel) {
    case 0:
        return RockType::NORMAL_ROCK;
    case 1:
        return RockType::HEAVY_ROCK;
    case 2:
        return RockType::SNOW_ROCK;
    case 3:
        return RockType::ICE_ROCK;
    case 4:
        return RockType::LAVA_ROCK;
    default:
        return RockType::CRYSTAL_ROCK;
    }
}

void RockSpawner::reset() { this->nextSpawnTime = 0.0; }

floatType RockSpawner::getRandAngularMomentum() const {
    return randomGenerator->getRandomRealNumber(gameConstants.rockSpawnerConstants.minAngularMomentum,
                                                gameConstants.rockSpawnerConstants.maxAngularMomentum);
}
