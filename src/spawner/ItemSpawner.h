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

class ItemSpawner : public Singleton<ItemSpawner> {
    friend class Singleton<ItemSpawner>; // Allow Singleton to access the constructor??

  public:
    /**
     * This method spawns items.
     * @param world
     */
    void spawnItems();

  private:
    static floatType nextSpawnTime;

    World &world = World::getInstance();
    RandomGenerator &randomGenerator = RandomGenerator::getInstance();

    void updateNextSpawnTime();
    ItemType getNextRandomItemType();
    Vector getNextRandomPosition();

    ItemSpawner();
    ~ItemSpawner();
};

#endif // SURVIVING_SARNTAL_ITEMSPAWNER_H
