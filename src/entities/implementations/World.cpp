//
// Created by Anietta Weckauff on 06.05.24.
//
#include "../World.h"

#include <iostream>
#include <mutex>

World::World() { std::cout << "World constructed" << std::endl; }

float World::getMaxX() const { return maxX; }

void World::setMaxX(const float maxX) { this->maxX = maxX; }

Hiker &World::getHiker() const { return *hiker; }

Inventory &World::getInventory() const { return *inventory; }

Monster &World::getMonster() const { return monster; }

Mountain &World::getMountain() const { return mountain; }

bool World::isOutOfScope(RenderedEntity &entity) const {
    bool result = entity.getPosition().x < this->minX - Mountain::CHUNK_WIDTH;
    // entity.getPosition().x > this->maxX + Mountain::CHUNK_WIDTH || entity.getPosition().y < -10000 ||
    // entity.getPosition().y > mountain.getYPosFromX(entity.getPosition().x);
    if (result) {
        std::cout << "A rock has left this cruel world" << entity.getPosition().x << entity.getPosition().y
                  << std::endl;
    }
    return result;
}

std::list<std::shared_ptr<Item>> World::getNearbyItems() const {
    std::list<std::shared_ptr<Item>> nearbyItems;
    for (const auto &item : *this->items) {
        if (item->getPosition().distanceTo(this->hiker->getPosition()) < HIKER_ITEM_COLLECTION_RANGE) {
            nearbyItems.push_back(item);
        }
    }
    return nearbyItems;
}

void World::addRock(Rock &rock) { this->rocks->push_back(rock); }

std::list<Rock> &World::getRocks() { return *rocks; }

std::list<std::shared_ptr<Item>> &World::getItems() const { return *items; }

void World::addItem(const Item &item) const { items->push_back(std::make_unique<Item>(item)); }

void World::useItem(const ItemType itemType) {
    switch (itemType) {
    case KAISERSCHMARRN:
        useKaiserschmarrn();
        break;
    case COIN:
        useCoin();
        break;
    case DUCK_ITEM:
        useDuck();
        break;
    default:
        break;
    }
}

void World::useKaiserschmarrn() const {
    const int currentHealth = this->hiker->getHealthPoints();
    this->hiker->setHealthPoints(std::min(currentHealth + KAISERSCHMARRN_HEALTH_RESTORATION, HIKER_MAX_HEALTH));
    std::cout << "used Kaiserschmarrn" << std::endl;
}

void World::useCoin() { // NOLINT(*-convert-member-functions-to-static)
    this->coinScore += COIN_SCORE;
    std::cout << "used coin" << std::endl;
}

void World::useDuck() { // NOLINT(*-convert-member-functions-to-static)
    //  TODO play sounds
    std::cout << "used duck" << std::endl;
}

float World::getMinX() const { return minX; }
void World::setMinX(const float minX) { World::minX = minX; }

World::~World() { std::cout << "World destroyed" << std::endl; }