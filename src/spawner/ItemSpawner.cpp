//
// Created by Anietta Weckauff on 14.05.24.
//
#include "ItemSpawner.h"
#include "spdlog/spdlog.h"

// NOLINTBEGIN
floatType ItemSpawner::nextSpawnTime(SPAWN_START_TIME);
// NOLINTEND

ItemSpawner::ItemSpawner(World &world) : world(world) {
    YAML::Node items = ConfigManager::getInstance().getItems();
    this->spawnWeightsSum = 0;
    for (YAML::const_iterator it = items.begin(); it != items.end(); ++it) {
        int spawnWeight = it->second["spawn-weight"].as<int>();
        int typeId = it->second["type-id"].as<int>();
        this->spawnWeightsSum += spawnWeight;
        this->spawnWeights[typeId] = spawnWeight;
    }
}

void ItemSpawner::spawnItems() {
    if (GetTime() < nextSpawnTime) {
        return;
    }

    const auto itemType = getNextRandomItemType();
    const auto position = getNextRandomPosition();

    const Item newItem = Item(itemType, position);
    this->world.addItem(newItem);
    updateNextSpawnTime();

    spdlog::info("Spawning {0} at position (x: {1}, y: {2}).", newItem.getName(), position.x, position.y);
}
void ItemSpawner::updateNextSpawnTime() {
    auto rand = static_cast<floatType>(randomGenerator.getRandomNumber(2, 10));
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
    auto randYOffset = static_cast<floatType>(randomGenerator.getRandomNumber(50, 300));
    auto xPosition = static_cast<floatType>(this->world.getMaxX() + 10);
    auto yPosition = static_cast<floatType>(this->world.getMountain().getYPosFromX(xPosition)) - randYOffset;
    spdlog::debug("Next spawn position is (x: {0}, y: {1}", xPosition, yPosition);
    return Vector{xPosition, yPosition};
}
