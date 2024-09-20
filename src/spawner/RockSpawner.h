//
// Created by Anietta Weckauff on 13.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCKSPAWNER_H
#define SURVIVING_SARNTAL_ROCKSPAWNER_H

#include "../entities/World.h"
#include "../game/GameProperties.hpp"
#include "../utilities/Singleton.hpp"

/**
 * Rock spawning phases that determine the spawn velocity and spawn time interval.
 */
enum RockSpawnDifficulty { EEEEEASY = 0, EASY = 1, MEDIUM = 2, HARD = 3, I_WANT_MOMY = 4 };

/**
 * This class is responsible for spawning rocks based on the current phase and state of the game.
 */
// TODO extract all the constants
class RockSpawner {
  public:
    explicit RockSpawner(World &world, GameConstants &gameConstants);
    ~RockSpawner() = default;
    /**
     * This method spawns rocks.
     */
    void spawnRocks();

  private:
    GameConstants &gameConstants;
    floatType lastSpawnTime{0.};
    int numberOfRocksSinceLastBatch{0};
    bool b = false;
    RockSpawnDifficulty rockSpawnDifficulty = EEEEEASY;
    RandomGenerator *randomGenerator = &RandomGenerator::getInstance();

    World &world;

    /**
     * Determines the number of rocks to spawn based on the current rock spawn
     * phase.
     * @param rockSpawnPhase
     * @param spawnData
     * @return number of rocks to spawn
     */
    int computeNumRocksToSpawn();

    /**
     * Determines the time between rock spawns based on the current rock spawn difficulty.
     * @param rockSpawnPhase
     * @return time between rock spawns
     */
    floatType rockSpawnTimeFromPhase();

    /**
     * Determines the current rock spawn difficulty based on how far the hiker has
     * progressed in the game (measured in the horizontal distance).
     * @return rock spawn difficulty
     */
    RockSpawnDifficulty determineRockSpawnDifficulty() const;

    /**
     * Determines the current rock type based on how far the hiker has
     * progressed in the game (measured in the horizontal distance).
     * @return rock spawn difficulty
     */
    RockType determineRockTypePhase();

    bool shouldSpawnRocks();

    void spawnRock(size_t idxRock);

    std::vector<Vector> getOffsetsAdditionalRocks();

    /**
     * This method generates a random radius between maxRockSize/2 und maxRockSize.
     * @return random radius
     */
    floatType getRandRadius() const;

    /**
     * This method generates a random position where a rock is spawned.
     * @return random position
     */
    Vector getRandSpawnPos() const;

    /**
     * This method generates a random velocity based on the current rock spawn phase.
     * @return random velocity
     */
    Vector getRandVelocity() const;

    /**
     * This method generates a random number that is used as the number of points
     * for generating a polygon.
     * @return random number of points
     */
    int getRandPointNumber() const;

    /**
     * This method generates a random density value.
     * @return random density
     */
    floatType getRandDensity() const;

    /**
     *
     * @return
     */
    void createRock(Vector &position, DynamicPolygon &polygon, DynamicProperties &dynamicProperties);

    Vector getRockSpawnPosition(size_t idxRock);
    DynamicPolygon getRandDynamicPolygon(Vector position);
    floatType determineDifficultyFactor() const;
    floatType determineLowerBoundVelocity() const;
};

#endif // SURVIVING_SARNTAL_ROCKSPAWNER_H
