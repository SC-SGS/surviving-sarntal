//
// Created by Anietta Weckauff on 13.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCKSPAWNER_H
#define SURVIVING_SARNTAL_ROCKSPAWNER_H

#include "../entities/World.h"
#include "../game/DifficultyService.hpp"
#include "../game/GameProperties.hpp"
#include "../utilities/RandomGenerator.hpp"

/**
 * Rock spawning phases that determine the spawn velocity and spawn time interval.
 */
enum RockSpawnDifficulty { EEEEEASY = 0, EASY = 1, MEDIUM = 2, HARD = 3, I_WANT_MOMY = 4 };

/**
 * This class is responsible for spawning rocks based on the current phase and state of the game.
 */
class RockSpawner {
  public:
    RockSpawner(World &world, DifficultyService &difficultyService, GameConstants &gameConstants);
    ~RockSpawner() = default;
    /**
     * This method spawns rocks.
     */
    void spawnRocks();

    void reset();

  private:
    World &world;
    DifficultyService &difficultyService;
    GameConstants &gameConstants;
    floatType lastSpawnTime{0.};
    floatType nextSpawnTime{0.};
    int numberOfRocksSinceLastBatch{0};
    bool b = false;
    RockSpawnDifficulty rockSpawnDifficulty = EEEEEASY;
    RandomGenerator *randomGenerator = &RandomGenerator::getInstance();

    /**
     * Determines the number of rocks to spawn based on the current rock spawn
     * phase.
     * @return number of rocks to spawn
     */
    int computeNumRocksToSpawn() const;

    /**
     * Determines the time between rock spawns based on the current rock spawn difficulty.
     * @return time between rock spawns
     */
    floatType rockSpawnTimeFromPhase() const;

    /**
     * Determines the current rock type based on how far the hiker has
     * progressed in the game (measured in the horizontal distance).
     * @return rock spawn difficulty
     */
    RockType determineRockTypePhase() const;

    bool shouldSpawnRocks() const;

    void spawnRock(size_t idxRock) const;

    std::vector<Vector> getOffsetsAdditionalRocks() const;

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
    Vector getRandLinearMomentum() const;

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
     * This method generates a random angular momentum.
     * @return random angular momentum
     */
    floatType getRandAngularMomentum() const;

    /**
     *
     * @param position
     * @param poly
     * @param dynamicProperties
     */
    void createRock(const Vector &position,
                    const DynamicConvexPolygon &poly,
                    const DynamicProperties &dynamicProperties) const;

    Vector getRockSpawnPosition(size_t idxRock) const;
    DynamicConvexPolygon getRandDynamicPolygon(const Vector &position) const;
    floatType determineDifficultyFactor() const;
    floatType determineLowerBoundLinearMomentum() const;
    void updateNextSpawnTime();
};

#endif // SURVIVING_SARNTAL_ROCKSPAWNER_H
