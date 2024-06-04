//
// Created by Anietta Weckauff on 14.05.24.
//

#ifndef SURVIVING_SARNTAL_ITEMSPAWNER_H
#define SURVIVING_SARNTAL_ITEMSPAWNER_H

#include "../entities/World.h"
#include "../utilities/Singleton.hpp"

/**
 * This class is responsible for spawning items during the game.
 */

class ItemSpawner : public Singleton<ItemSpawner> {
    friend class Singleton<ItemSpawner>; // Allow Singleton to access the constructor??

  public:
    /**
     * generate a random variable that follows the geometric distribution with p
     * = 1/2^p_exp
     * @param p_exp
     * @return
     */
    static int generateRandomNumberGeometric(int p_exp);

    /**
     * This method spawns items.
     * @param world
     */
    void spawnItems();

  private:
    float itemSpawnTime{};
    int itemCount{3};
    static double nextSpawnTime;

    static std::random_device dev;
    static std::mt19937 randomEngine;
    static std::uniform_int_distribution<int> distribution;

    World &world = World::getInstance();

    static void updateNextSpawnTime();
    static ItemType getNextRandomItemType();
    static Vector getNextRandomPosition();

    ItemSpawner();
    ~ItemSpawner();
};

#endif // SURVIVING_SARNTAL_ITEMSPAWNER_H
