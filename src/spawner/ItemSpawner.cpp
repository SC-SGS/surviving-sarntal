//
// Created by Anietta Weckauff on 14.05.24.
//
#include "ItemSpawner.h"
#include "spdlog/spdlog.h"

ItemSpawner::ItemSpawner(World &world, GameConstants gameConstants, std::unordered_map<ItemType, ItemDto> &itemDtoMap)
    : world(world), gameConstants(gameConstants), nextSpawnTime(gameConstants.itemsConstants.startSpawnTime),
      itemDtoMap(itemDtoMap), spawnWeightsSum(static_cast<int>(itemDtoMap.size())) {
    this->nextSpawnTime = this->gameConstants.itemsConstants.startSpawnTime;
    const auto &items = ConfigManager::getInstance().getItems();
    for (const auto &item : items) {
        int itemType = item.first;
        int itemSpawnWeight = item.second.spawnWeight;
        this->spawnWeights[itemType] = itemSpawnWeight;
        this->spawnWeightsSum += itemSpawnWeight;
    }
}

void ItemSpawner::spawnItems() {
    if (GetTime() < nextSpawnTime) {
        return;
    }

    const auto itemType = getNextRandomItemType();
    const auto position = getNextRandomPosition();

    const Item newItem = Item(itemType, position, gameConstants.itemsConstants.itemBaseHeight, itemDtoMap[itemType]);
    this->world.addItem(newItem);
    updateNextSpawnTime();

    spdlog::info("Spawning {0} at position (x: {1}, y: {2}).", newItem.getName(), position.x, position.y);
}
void ItemSpawner::updateNextSpawnTime() {
    auto rand = static_cast<floatType>(randomGenerator.getRandomNumber(2, 10)); // todo get range from config
    nextSpawnTime = static_cast<floatType>(GetTime()) + rand;
    spdlog::debug("Next spawn time was set to: {}", nextSpawnTime);
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
    auto randYOffset = static_cast<floatType>(randomGenerator.getRandomNumber(50, 300)); // todo get range from config
    auto xPosition = static_cast<floatType>(this->world.getMaxX() + 10);
    auto yPosition = static_cast<floatType>(this->world.getMountain().calculateYPos(xPosition)) + randYOffset;
    spdlog::debug("Next spawn position is (x: {0}, y: {1}", xPosition, yPosition);
    return Vector{xPosition, yPosition};
}
