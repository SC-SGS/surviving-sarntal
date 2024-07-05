//
// Created by Anietta Weckauff on 14.05.24.
//

#ifndef SURVIVING_SARNTAL_ITEMSPAWNER_H
#define SURVIVING_SARNTAL_ITEMSPAWNER_H

#include "../entities/World.h"
#include "../utilities/RandomGenerator.hpp"
#include "../utilities/Singleton.hpp"

/**
 * This class is responsible for spawning items during the game.
 */

class ItemSpawner {

  public:
    explicit ItemSpawner(World &world);
    ~ItemSpawner() = default;
    /**
     * This method spawns items.
     * @param world
     */
    void spawnItems();

  private:
    static floatType nextSpawnTime;
    int spawnWeightsSum;

    /**
     * Maps the itemType id to the spawn weight of the item
     */
    std::unordered_map<int, int> spawnWeights;

    World &world;
    RandomGenerator &randomGenerator = RandomGenerator::getInstance();

    void updateNextSpawnTime();
    ItemType getNextRandomItemType();
    Vector getNextRandomPosition();
};

#endif // SURVIVING_SARNTAL_ITEMSPAWNER_H
