//
// Created by Anietta Weckauff on 14.05.24.
//
#include "ItemSpawner.h"
#include "spdlog/spdlog.h"

ItemSpawner::ItemSpawner(World &world, GameConstants &gameConstants, std::unordered_map<ItemType, ItemDto> &itemDtoMap)
    : world(world),
      gameConstants(gameConstants),
      nextSpawnDistance(gameConstants.itemsConstants.startSpawnTime),
      itemDtoMap(itemDtoMap),
      spawnWeightsSum(static_cast<int>(itemDtoMap.size())) {
    this->nextSpawnDistance = this->gameConstants.itemsConstants.startSpawnTime;
    const auto &items = ConfigManager::getInstance().getItems();
    for (const auto &item : items) {
        int itemType = item.first;
        int itemSpawnWeight = item.second.spawnWeight;
        this->spawnWeights[itemType] = itemSpawnWeight;
        this->spawnWeightsSum += itemSpawnWeight;
    }
}

void ItemSpawner::spawnItems() {
    bool skipSpawn = this->world.getMaxX() < nextSpawnDistance || !this->gameConstants.itemsConstants.spawnItems;
    if (skipSpawn) {
        return;
    }

    const auto itemType = getNextRandomItemType();
    const auto position = getNextRandomPosition();

    const Item newItem = Item(itemType, position, gameConstants.itemsConstants.itemBaseHeight, itemDtoMap[itemType]);
    this->world.addItem(newItem);
    updateNextSpawnDistance();

    spdlog::info("Spawning {0} at position (x: {1}, y: {2}).", newItem.getName(), position.x, position.y);
}
void ItemSpawner::updateNextSpawnDistance() {
    auto rand = static_cast<floatType>(randomGenerator.getRandomNumber(2, 6));
    nextSpawnDistance = this->world.getMaxX() + rand;
    spdlog::debug("Next spawn distance was set to: {}", nextSpawnDistance);
}
ItemType ItemSpawner::getNextRandomItemType() {
    int rand = randomGenerator.getRandomNumber(1, this->spawnWeightsSum);
    for (auto spawnWeight : this->spawnWeights) {
        rand -= spawnWeight.second;
        if (rand < 1) {
            spdlog::debug("Next item to be spawned has item type: {}", spawnWeight.first);
            return ItemType(spawnWeight.first);
        }
    }
    return COIN;
}
Vector ItemSpawner::getNextRandomPosition() {
    int minY = static_cast<int>(gameConstants.itemsConstants.itemBaseHeight);
    int maxY = static_cast<int>(gameConstants.itemsConstants.itemMaxHeight);
    auto randYOffset = static_cast<floatType>(randomGenerator.getRandomNumber(minY, maxY));
    auto xPosition = static_cast<floatType>(this->world.getMaxX() + 1);
    auto yPosition = static_cast<floatType>(this->world.getTerrain().getMaxHeight(xPosition)) + randYOffset;
    spdlog::debug("Next spawn position is (x: {0}, y: {1}", xPosition, yPosition);
    return Vector{xPosition, yPosition};
}
