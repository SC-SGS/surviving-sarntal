//
// Created by Anietta Weckauff on 13.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCKSPAWNER_H
#define SURVIVING_SARNTAL_ROCKSPAWNER_H

#include "../entities/World.h"

struct SpawnData {
    float rockSpawnTime{0.};
    int batchModuloCount{0};
    int explosiveRockModuloCount{0};
};

/**
 * Rock spawning phases that can occur during the game.
 */

enum RockSpawnPhase { VERY_BEGINNING, IRREGULAR_ROCKS, REGULAR_ROCKS, ROCK_BATCHES, EXPLOSIVE_BATCHES };

/**
 * This class is responsible for spawning rocks based on the current phase and state of the game.
 */

class RockSpawner {
  private:
  public:
    /**
     * This method spawns rocks.
     *
     * @param world
     * @param spawnData
     */
    static void spawnRocks(World *world, SpawnData *spawnData);

    /**
     * Determines the number of rocks to spawn based on the current rock spawn
     * phase.
     * @param rockSpawnPhase
     * @param spawnData
     * @return number of rocks to spawn
     */
    static int computeNumRocksToSpawn(RockSpawnPhase rockSpawnPhase, SpawnData *spawnData);
    /**
     * Determines the time between rock spawns based on the current rock spawn
     * phase.
     * @param rockSpawnPhase
     * @return time between rock spawns
     */
    static float rockSpawnTimeFromPhase(RockSpawnPhase rockSpawnPhase);

    /**
     * Determines the current rock spawn phase based on the game time.
     * @param gameTime
     * @return rock spawn phase
     */
    static RockSpawnPhase determineRockSpawnPhase(double gameTime);
};

#endif // SURVIVING_SARNTAL_ROCKSPAWNER_H
