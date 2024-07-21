//
// Created by Anietta Weckauff on 13.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCKSPAWNER_H
#define SURVIVING_SARNTAL_ROCKSPAWNER_H

#include "../entities/World.h"
#include "../game/GameProperties.hpp"
#include "../utilities/Singleton.hpp"

/**
 * Rock spawning phases that can occur during the game.
 */
enum RockSpawnPhase { VERY_BEGINNING, IRREGULAR_ROCKS, REGULAR_ROCKS, ROCK_BATCHES, EXPLOSIVE_BATCHES };

/**
 * This class is responsible for spawning rocks based on the current phase and state of the game.
 */
// TODO extract all the constants
class RockSpawner {
  public:
    explicit RockSpawner(World &world, GameConstants gameConstants);
    ~RockSpawner() = default;
    /**
     * This method spawns rocks.
     */
    void spawnRocks();

  private:
    GameConstants gameConstants;
    floatType lastSpawnTime{0.};
    int numberOfRocksSinceLastBatch{0};
    bool b = false;
    RockSpawnPhase rockSpawnPhase = VERY_BEGINNING;

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
     * Determines the time between rock spawns based on the current rock spawn
     * phase.
     * @param rockSpawnPhase
     * @return time between rock spawns
     */
    floatType rockSpawnTimeFromPhase();

    /**
     * Determines the current rock spawn phase based on the game time.
     * @param gameTime
     * @return rock spawn phase
     */
    static RockSpawnPhase determineRockSpawnPhase();

    bool shouldSpawnRocks();

    void spawnRock(size_t idxRock);

    std::vector<Vector> getOffsetsAdditionalRocks();

    Vector getRandSpawnPos() const;
};

#endif // SURVIVING_SARNTAL_ROCKSPAWNER_H
