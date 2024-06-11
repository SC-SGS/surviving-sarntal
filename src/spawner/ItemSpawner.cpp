//
// Created by Anietta Weckauff on 14.05.24.
//
#include "ItemSpawner.h"
#include "../entities/Item.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>

// NOLINTBEGIN
floatType ItemSpawner::nextSpawnTime(SPAWN_START_TIME);
// NOLINTEND

ItemSpawner::ItemSpawner() { spdlog::info("ItemSpawner was initialized."); }

ItemSpawner::~ItemSpawner() { spdlog::info("ItemSpawner was destroyed."); }

void ItemSpawner::spawnItems() {
    if (GetTime() < nextSpawnTime) {
        return;
    }

    const auto itemType = getNextRandomItemType();
    const auto position = getNextRandomPosition();

    const Item newItem = Item(itemType, position);
    this->world.addItem(newItem);
    updateNextSpawnTime();

    spdlog::info("Spawning {0} at position (x: {1}, y: {2}).", itemType, position.x, position.y);
}
void ItemSpawner::updateNextSpawnTime() {
    auto rand = static_cast<floatType>(randomGenerator.getRandomNumber(2, 10));
    nextSpawnTime = static_cast<floatType>(GetTime()) + rand;
    spdlog::debug("Next spawn time was set to: {}", nextSpawnTime);
}
ItemType ItemSpawner::getNextRandomItemType() {
    int rand = randomGenerator.getRandomNumber(0, 2);
    spdlog::debug("Next item to be spawned has item type: {}", rand);
    switch (rand) {
    case 0:
        return KAISERSCHMARRN;
    case 1:
        return COIN;
    case 2:
        return DUCK_ITEM;
    default:
        return NO_ITEM;
    }
}
Vector ItemSpawner::getNextRandomPosition() {
    auto randYOffset = static_cast<floatType>(randomGenerator.getRandomNumber(50, 300));
    auto xPosition = static_cast<floatType>(World::getInstance().getMaxX() + 10);
    auto yPosition = static_cast<floatType>(Mountain::getInstance().getYPosFromX(xPosition)) - randYOffset;
    spdlog::debug("Next spawn position is (x: {0}, y: {1}", xPosition, yPosition);
    return Vector{xPosition, yPosition};
}
