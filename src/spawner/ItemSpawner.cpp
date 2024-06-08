//
// Created by Anietta Weckauff on 14.05.24.
//
#include "ItemSpawner.h"
#include "../entities/Item.hpp"
#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>

// NOLINTBEGIN
double ItemSpawner::nextSpawnTime(SPAWN_START_TIME);
// NOLINTEND

ItemSpawner::ItemSpawner() { std::cout << "ItemSpawner initialized." << std::endl; }

ItemSpawner::~ItemSpawner() { std::cout << "ItemSpawner destroyed." << std::endl; }

void ItemSpawner::spawnItems() {
    if (GetTime() < nextSpawnTime) {
        return;
    }

    const auto itemType = getNextRandomItemType();
    const auto position = getNextRandomPosition();

    const Item newItem = Item(itemType, position);
    this->world.addItem(newItem);
    updateNextSpawnTime();

    std::cout << "spawning " << itemType << " at " << position.x << "," << position.y << " " << std::endl;
}
void ItemSpawner::updateNextSpawnTime() {
    auto rand = static_cast<float>(randomGenerator.getRandomNumber(2, 10));
    nextSpawnTime = GetTime() + rand;
}
ItemType ItemSpawner::getNextRandomItemType() {
    int rand = randomGenerator.getRandomNumber(0, 2);
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
    return Vector{xPosition, yPosition};
}
