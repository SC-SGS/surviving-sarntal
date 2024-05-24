//
// Created by Anietta Weckauff on 14.05.24.
//

#ifndef SURVIVING_SARNTAL_ITEMSPAWNER_H
#define SURVIVING_SARNTAL_ITEMSPAWNER_H

#include "../entities/World.h"

/**
 * This class is responsible for spawning items during the game.
 */

class ItemSpawner {
  private:
    float itemSpawnTime{};
    int itemCount{3};

    static std::random_device dev;
    static std::mt19937 randomEngine;
    static std::uniform_int_distribution<int> distribution;

  public:
    ItemSpawner();
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
    void spawnItems(World *world);
};

#endif // SURVIVING_SARNTAL_ITEMSPAWNER_H
