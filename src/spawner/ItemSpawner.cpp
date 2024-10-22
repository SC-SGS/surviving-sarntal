//
// Created by Anietta Weckauff on 14.05.24.
//
#include "ItemSpawner.h"
#include "spdlog/spdlog.h"

ItemSpawner::ItemSpawner(World &world, GameConstants &gameConstants, std::unordered_map<ItemType, ItemDto> &itemDtoMap)
    : world(world),
      gameConstants(gameConstants),
      nextSpawnTime(gameConstants.itemsConstants.startSpawnTime),
      itemDtoMap(itemDtoMap),
      spawnWeightsSum(static_cast<int>(itemDtoMap.size())) {
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
    if (this->shouldSpawn()) {
        const Item newItem = this->getNextSpawnItem();
        if (newItem.getItemType() == NO_ITEM) {
            return;
        }
        this->world.addItem(newItem);
        updateNextSpawnTime();

        spdlog::info("Spawning {0} at position (x: {1}, y: {2}).", newItem.getName(), newItem.getPosition().x,
                     newItem.getPosition().y);
    }
}

bool ItemSpawner::shouldSpawn() const {
    double gamePlayTime = DifficultyService::getInstance().getElapsedGamePlayTime();
    bool timeToSpawn = this->nextSpawnTime <= gamePlayTime;
    bool spawningOn = this->gameConstants.itemsConstants.spawnItems;
    return timeToSpawn && spawningOn;
}

Item ItemSpawner::getNextSpawnItem() {
    const auto itemType = getNextRandomItemType();
    const auto position = getNextRandomPosition();
    return {itemType, position, gameConstants.itemsConstants.itemBaseSpawnHeight, itemDtoMap[itemType]};
}

void ItemSpawner::updateNextSpawnTime() {
    int minSpawnTime = this->gameConstants.itemsConstants.minSpawnTime;
    int maxSpawnTime = this->gameConstants.itemsConstants.maxSpawnTime;
    auto randSpawnTime = static_cast<floatType>(randomGenerator.getRandomNumber(minSpawnTime, maxSpawnTime));
    this->nextSpawnTime = DifficultyService::getInstance().getElapsedGamePlayTime() + randSpawnTime;
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
    return NO_ITEM; // Default to Coin
}

Vector ItemSpawner::getNextRandomPosition() {
    int minY = static_cast<int>(gameConstants.itemsConstants.itemBaseSpawnHeight);
    int maxY = static_cast<int>(gameConstants.itemsConstants.itemMaxSpawnHeight);
    auto randYOffset = static_cast<floatType>(randomGenerator.getRandomNumber(minY, maxY));

    auto xPosition = static_cast<floatType>(this->world.getMaxX() + 1); // We add 1 so that we spawn off screen
    auto yPosition = static_cast<floatType>(this->world.getTerrain().getMaxHeight(xPosition)) + randYOffset;

    spdlog::debug("Next spawn position is (x: {0}, y: {1}", xPosition, yPosition);

    return Vector{xPosition, yPosition};
}
void ItemSpawner::reset() { this->nextSpawnTime = 0.0; }
