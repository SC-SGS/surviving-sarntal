//
// Created by Anietta Weckauff on 06.05.24.
//
#include "../World.h"
#include "../../output/graphics/Renderer.h"

#include <iostream>
#include <mutex>

World::World(Terrain &terrain,
             Hiker &hiker,
             Inventory &inventory,
             Monster &monster,
             AudioService &audioService,
             GameConstants &gameConstants)
    : terrain(terrain),
      hiker(hiker),
      inventory(inventory),
      monster(monster),
      audioService(audioService),
      gameConstants(gameConstants) {}

World::~World() = default;

floatType World::getMaxX() const { return maxX; }

void World::setMaxX(const floatType maxX) { this->maxX = maxX; }

Hiker &World::getHiker() const { return hiker; }

Inventory &World::getInventory() const { return inventory; }

Monster &World::getMonster() const { return monster; }

Terrain &World::getTerrain() const { return terrain; }

bool World::isOutOfScope(const RenderedEntity &entity) const {
    bool result = entity.getPosition().x < this->minX - this->gameConstants.terrainConstants.bufferLeft +
                                               2 * this->gameConstants.rockConstants.maxRockSize;
    const floatType rightBorder = this->terrain.getRightBorder();
    result = entity.getPosition().x > rightBorder - 2 * this->gameConstants.rockConstants.maxRockSize || result;
    if (result) {
        spdlog::debug("An entity has left the scope of the game.");
    }
    return result;
}

// TODO this should be hiker specific
std::list<std::shared_ptr<Item>> World::getNearbyItems() const {
    std::list<std::shared_ptr<Item>> nearbyItems;
    const Vector &position = this->hiker.getPosition();
    const auto adjustedHikerPosition = Vector{position.x, position.y + this->hiker.getHeight() / 2};
    for (const auto &item : *this->items) {
        bool inRange =
            item->getPosition().distanceTo(adjustedHikerPosition) < this->gameConstants.itemsConstants.collectionRadius;
        for (const auto &vertex : hiker.getCurrentHitbox()->getWorldSpaceVertices()) {
            if (item->getPosition().distanceTo(vertex) < this->gameConstants.itemsConstants.collectionRadius) {
                inRange = true;
            }
        }
        if (inRange) {
            nearbyItems.push_back(item);
        }
    }
    return nearbyItems;
}

void World::addRock(const std::shared_ptr<Rock> &rock) const { this->rocks->push_back(rock); }

std::list<std::shared_ptr<Rock>> &World::getRocks() const { return *rocks; }

std::list<std::shared_ptr<Item>> &World::getItems() const { return *items; }

void World::clearAllItems() { this->items->clear(); }

void World::clearAllRocks() { this->rocks->clear(); }

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
    case ROCK_BOMB:
        useRockBomb();
    default:
        break;
    }
}

void World::useKaiserschmarrn() const {
    this->hiker.addHealthPoints(gameConstants.itemsConstants.kaiserschmarrnHealthRestoration);
    this->audioService.playSound("eat");
    spdlog::debug("Used Kaiserschmarrn.");
}

void World::useCoin() { // NOLINT(*-convert-member-functions-to-static)
    this->coinScore += gameConstants.itemsConstants.coinScore;
    this->audioService.playSound("use-coin");
    spdlog::debug("Used Coin.");
}

void World::useDuck() { // NOLINT(*-convert-member-functions-to-static)
    this->audioService.playSound("use-duck");
    this->hiker.setShield(gameConstants.itemsConstants.shieldTime);
    spdlog::debug("Used Duck.");
}

floatType World::getMinX() const { return minX; }
void World::setMinX(const floatType newMinX) { this->minX = newMinX; }

int World::getCoinScore() const { return this->coinScore; }

Vector World::getGameScore() const { return this->gameScore; }

void World::updateGameScore() {
    const float hikerHeight = this->hiker.getPosition().y;
    this->gameScore.y = std::max(this->gameScore.y, hikerHeight);
    const float hikerXDistance = this->hiker.getPosition().x;
    this->gameScore.x = std::max(this->gameScore.x, hikerXDistance);
}

void World::reset() {
    this->minX = 0;
    this->maxX = graphics::SCREEN_WIDTH_IN_PIXEL / graphics::UNIT_TO_PIXEL_RATIO;
    this->coinScore = 0;
    this->gameScore = {0, 0};
    this->terrain.reset();
    this->resetHiker();
    this->resetMonster();
    this->resetAttributes();
}

void World::clearRocks() {
    for (auto &rock : *this->rocks) {
        rock->setShouldBeDestroyed(true);
    }
}

void World::resetHiker() const {
    floatType hikerPositionX =
        this->gameConstants.hikerConstants.spawnXRelativeToScreenWidth * (this->getMaxX() - this->getMinX()) +
        this->getMinX();
    const floatType hikerPositionY =
        terrain.getMaxHeight(hikerPositionX) + this->gameConstants.physicsConstants.epsilon;
    const Vector pos = {hikerPositionX, hikerPositionY};
    this->hiker.reset(pos);
}

void World::resetMonster() const {
    this->getMonster().setXPosition(this->gameConstants.barriersConstants.monsterXRelativeToScreenWidth *
                                        (this->getMaxX() - this->getMinX()) +
                                    this->getMinX());
}

void World::resetAttributes() {
    this->setMinX(0);
    this->setMaxX(graphics::SCREEN_WIDTH_IN_PIXEL / graphics::UNIT_TO_PIXEL_RATIO);
    this->coinScore = 0;
    this->gameScore.x = this->hiker.getPosition().x;
    this->gameScore.y = this->hiker.getPosition().y;
    this->items->clear();
    this->rocks->clear();
    this->inventory.reset();
}
void World::useRockBomb() {
    this->clearRocks();
    audioService.playSound("nuke");
}

void World::useSelectedItem() {
    if (!this->getInventory().selectedSlotIsEmpty()) {
        this->useItem(this->getInventory().getSelectedItemType());
        this->getInventory().removeSelectedItem();
    }
}