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
    explicit ItemSpawner(World &world, GameConstants &gameConstants, std::unordered_map<ItemType, ItemDto> &itemDtoMap);
    ~ItemSpawner() = default;
    /**
     * This method spawns items.
     * @param world
     */
    void spawnItems();

    void reset();

  private:
    // Dependencies
    World &world;
    RandomGenerator &randomGenerator = RandomGenerator::getInstance();

    // Attributes
    GameConstants &gameConstants;
    floatType nextSpawnDistance;
    int spawnWeightsSum;
    /**
     * Maps the itemType id to the spawn weight of the item
     */
    std::unordered_map<int, int> spawnWeights;
    /**
     * Maps the itemType to the item dto
     */
    std::unordered_map<ItemType, ItemDto> itemDtoMap;

    // Helper functions
    void updateNextSpawnDistance();
    ItemType getNextRandomItemType();
    Vector getNextRandomPosition();
};

#endif // SURVIVING_SARNTAL_ITEMSPAWNER_H
