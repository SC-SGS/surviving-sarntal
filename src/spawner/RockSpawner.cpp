//
// Created by Anietta Weckauff on 13.05.24.
//

#include "RockSpawner.h"
#include "PolygonGenerator.h"

RockSpawner::RockSpawner(World &world, GameConstants &gameConstants) : gameConstants(gameConstants), world(world) {}

void RockSpawner::spawnRocks() {
    if (!shouldSpawnRocks()) {
        return;
    }

    const int numRocksToSpawn = computeNumRocksToSpawn();
    for (int i = 0; i < numRocksToSpawn; i++) {
        this->spawnRock(i);
    }
}

void RockSpawner::spawnRock(const size_t idxRock) const {
    const Vector linearMomentum = this->getRandLinearMomentum();
    Vector position = this->getRockSpawnPosition(idxRock);
    const DynamicConvexPolygon poly = this->getRandDynamicPolygon(position);
    const DynamicProperties dynamicProperties{position, 0, 0, linearMomentum, 1};
    this->createRock(position, poly, dynamicProperties);
    spdlog::debug("Rock spawned at position (x: {0}, y: {1})", position.x, position.y);
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

bool RockSpawner::shouldSpawnRocks() {
    const auto gameTime = GetTime();
    const floatType timeBetweenRockSpawns = this->rockSpawnTimeFromPhase();
    const bool isTimeToSpawn = gameTime > this->lastSpawnTime + timeBetweenRockSpawns;
    const bool shouldSpawn = isTimeToSpawn && this->gameConstants.rockConstants.spawnRocks;
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

floatType RockSpawner::rockSpawnTimeFromPhase() const {
    const std::vector<floatType> spawnTimeInterval = gameConstants.rockSpawnerConstants.rockSpawnTimeInterval;
    return spawnTimeInterval[this->determineRockSpawnDifficulty()];
}

RockSpawnDifficulty RockSpawner::determineRockSpawnDifficulty() const {
    const floatType currentXCoordinate = this->world.getMaxX();
    const std::vector<floatType> spawningPhase = gameConstants.rockSpawnerConstants.spawningPhase;
    for (int i = 0; i < spawningPhase.size(); i++) {
        if (currentXCoordinate < spawningPhase[i]) {
            return static_cast<RockSpawnDifficulty>(i);
        }
    }
    return static_cast<RockSpawnDifficulty>(spawningPhase.size() - 1);
}

std::vector<Vector> RockSpawner::getOffsetsAdditionalRocks() const {
    const auto &rockConst = gameConstants.rockConstants;
    return {
        {0., 0.},
        {static_cast<floatType>(rockConst.maxRockSize + 5.), static_cast<floatType>(rockConst.maxRockSize * 2 + 10.)},
        {static_cast<floatType>(-rockConst.maxRockSize - 5.), static_cast<floatType>(rockConst.maxRockSize * 2 + 10.)}};
}

Vector RockSpawner::getRandSpawnPos() const {
    const auto spawnXPos = this->world.getMaxX() + gameConstants.rockConstants.spawnOffsetX;
    const auto randYOffset = randomGenerator->getRandomRealNumber(6, 8); // TODO magic numbers

    // static_cast<floatType>(std::rand() / (1.0 * RAND_MAX)) * (4 - 3) + 3; // TODO these should be constants
    const auto spawnYPos = this->world.getTerrain().getMaxHeight(spawnXPos) + randYOffset;
    return Vector{spawnXPos, spawnYPos};
}

int RockSpawner::getRandPointNumber() const { return randomGenerator->getRandomNumber(50, 200); } // TODO magic numbers

floatType RockSpawner::getRandDensity() const {
    const auto &rockConst = gameConstants.rockConstants;
    return randomGenerator->getRandomRealNumber(rockConst.minRockDensity, rockConst.maxRockDensity);
}

Vector RockSpawner::getRandLinearMomentum() const {
    const floatType lowerBoundLinearMomentum = this->determineLowerBoundLinearMomentum();
    const floatType upperBoundLinearMomentum = this->gameConstants.rockConstants.maxSpawnLinearMomentum;
    const floatType xComponent =
        -1 * randomGenerator->getRandomRealNumber(lowerBoundLinearMomentum, upperBoundLinearMomentum);
    return {xComponent, 0};
}

floatType RockSpawner::determineLowerBoundLinearMomentum() const {
    auto const &rockConst = this->gameConstants.rockConstants;
    const floatType difficultyFactor = determineDifficultyFactor();
    return difficultyFactor * (rockConst.maxSpawnLinearMomentum - rockConst.minSpawnLinearMomentum) +
           rockConst.minSpawnLinearMomentum;
}

floatType RockSpawner::determineDifficultyFactor() const {
    const std::vector<floatType> linearMomentumDifficultyFactor =
        gameConstants.rockSpawnerConstants.linearMomentumDifficultyFactor;
    return linearMomentumDifficultyFactor[this->determineRockSpawnDifficulty()];
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
    const floatType currentXCoordinate = this->world.getMaxX();
    const std::vector<floatType> rockTypePhase = gameConstants.rockSpawnerConstants.rockTypePhase;
    for (int i = 0; i < rockTypePhase.size(); i++) {
        if (currentXCoordinate < rockTypePhase[i]) {
            return static_cast<RockType>(i);
        }
    }
    return static_cast<RockType>(0);
}
void RockSpawner::reset() {}
