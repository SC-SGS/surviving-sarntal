//
// Created by Anietta Weckauff on 06.05.24.
//
#include "../World.h"
#include "../../output/graphics/Renderer.h"

#include <iostream>
#include <mutex>

floatType World::getMaxX() const { return maxX; }

void World::setMaxX(const floatType maxX) { this->maxX = maxX; }

Hiker &World::getHiker() const { return hiker; }

Inventory &World::getInventory() const { return inventory; }

Monster &World::getMonster() const { return monster; }

Mountain &World::getMountain() const { return mountain; }

bool World::isOutOfScope(const RenderedEntity &entity) const {
    const bool result = entity.getPosition().x < this->minX - Mountain::CHUNK_WIDTH;
    // entity.getPosition().x > this->maxX + Mountain::CHUNK_WIDTH || entity.getPosition().y < -10000 ||
    // entity.getPosition().y > mountain.getYPosFromX(entity.getPosition().x);
    if (result) {
        spdlog::debug("A rock has left the scope of the game.");
    }
    return result;
}

// TODO this should be hiker specific
std::list<std::shared_ptr<Item>> World::getNearbyItems() const {
    std::list<std::shared_ptr<Item>> nearbyItems;
    const Vector &position = this->hiker.getPosition();
    const auto adjustedHikerPosition = Vector{position.x, position.y + this->hiker.getHeight() / 2};
    for (const auto &item : *this->items) {
        const bool inRange = item->getPosition().distanceTo(adjustedHikerPosition) < HIKER_ITEM_COLLECTION_RANGE;
        if (inRange) {
            nearbyItems.push_back(item);
        }
    }
    return nearbyItems;
}

void World::addRock(const Rock &rock) const { this->rocks->push_back(rock); }

std::list<Rock> &World::getRocks() const { return *rocks; }

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
    this->hiker.addHealthPoints(KAISERSCHMARRN_HEALTH_RESTORATION);
    this->audioService.playSound("use-kaiserschmarrn");
    spdlog::debug("Used Kaiserschmarrn.");
}

void World::useCoin() { // NOLINT(*-convert-member-functions-to-static)
    this->coinScore += COIN_SCORE;
    this->audioService.playSound("use-coin");
    spdlog::debug("Used Coin.");
}

void World::useDuck() { // NOLINT(*-convert-member-functions-to-static)
    this->audioService.playSound("use-duck");
    spdlog::debug("Used Duck.");
}

floatType World::getMinX() const { return minX; }
void World::setMinX(const floatType newMinX) { this->minX = newMinX; }

int World::getCoinScore() const { return this->coinScore; }

World::World(Mountain &mountain, Hiker &hiker, Inventory &inventory, Monster &monster, AudioService &audioService)
    : audioService(audioService), mountain(mountain), hiker(hiker), inventory(inventory), monster(monster) {}

int World::getGameScore() const { return this->gameScore; }

void World::updateGameScore() {
    const int hikerHeight = static_cast<int>(this->mountain.getYPosFromX(this->hiker.getPosition().x));
    this->gameScore = std::max(this->gameScore, hikerHeight);
}

World::~World() = default;