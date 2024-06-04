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
std::random_device dev;
std::mt19937 ItemSpawner::randomEngine(0);
std::uniform_int_distribution<int> ItemSpawner::distribution{0, RAND_MAX};
double ItemSpawner::nextSpawnTime(0);
// NOLINTEND

ItemSpawner::ItemSpawner() { std::cout << "ItemSpawner initialized." << std::endl; }

ItemSpawner::~ItemSpawner() { std::cout << "ItemSpawner destroyed." << std::endl; }

int ItemSpawner::generateRandomNumberGeometric(int p_exp) {
    int rand = RAND_MAX;
    for (int i = 0; i < p_exp; ++i) {
        rand = rand & distribution(randomEngine);
    }
    return std::floor(std::log(RAND_MAX) - std::log(rand));
}

void ItemSpawner::spawnItems() {
    if (GetTime() < nextSpawnTime) {
        return;
    }

    const auto itemType = getNextRandomItemType();
    const auto position = getNextRandomPosition();
    const auto itemInformation = Item::getItemInformation(itemType);

    const Item newItem(itemType, itemInformation, position);
    this->world.addItem(newItem);
    updateNextSpawnTime();

    std::cout << "spawning " << itemType << " at " << position.x << "," << position.y << " " << std::endl;
}
void ItemSpawner::updateNextSpawnTime() {
    // TODO make better
    nextSpawnTime = GetTime() + 3.0f;
}
ItemType ItemSpawner::getNextRandomItemType() {
    // TODO make better
    return ItemType::DUCK_ITEM;
}
Vector ItemSpawner::getNextRandomPosition() {
    // TODO make better / dependent on physics implementation
    auto position = World::getInstance().getHiker().getPosition();
    position.y -= 200;
    return position;
}
