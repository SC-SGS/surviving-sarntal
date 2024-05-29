//
// Created by Anietta Weckauff on 06.05.24.
//
#include "../World.h"

// World::World() {}

// TODO
void World::initializeWorld() {
    minX = 0.0;
    maxX = graphics::SCREEN_WIDTH;
}
float World::getMaxX() const { return maxX; }
void World::setMaxX(float maxX) { World::maxX = maxX; }

void World::setHiker(const Hiker &newHiker) { hiker = newHiker; }
Hiker &World::getHiker() { return hiker; }

void World::setInventory(const Inventory &newInventory) { inventory = newInventory; }
Inventory &World::getInventory() { return inventory; }
void World::setMonster(const Monster &newMonster) { monster = newMonster; }
Monster &World::getMonster() { return monster; }
void World::setMountain(const MountainClass &newMountain) { mountain = newMountain; }
MountainClass &World::getMountain() { return mountain; }

bool World::isOutOfScope(RenderedEntity &entity) const {
    return entity.getPosition().x < this->minX - MountainClass::CHUNK_WIDTH ||
           entity.getPosition().x > this->maxX + MountainClass::CHUNK_WIDTH;
}

// TODO !!!
std::list<Item> World::getNearbyItems() const { return {}; } // NOLINT(*-convert-member-functions-to-static)
void World::addRock(RockClass *rock) { rocks.push_back(rock); }
void World::setRocks(const std::list<RockClass *> &newRocks) { rocks = newRocks; }
std::list<RockClass *> &World::getRocks() { return rocks; }

void World::setItems(const std::list<Item> &items) { this->items = items; }
std::list<Item> &World::getItems() { return items; }
void World::addItem(const Item &item) { items.push_back(item); }

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

void World::useKaiserschmarrn() {
    // Hiker hiker = this->getHiker();
    const int currentHealth = this->hiker.getHealthPoints();
    this->hiker.setHealthPoints(std::min(currentHealth + KAISERSCHMARRN_HEALTH_RESTORATION, HIKER_MAX_HEALTH));
    std::cout << "used Kaiserschmarrn" << std::endl;
}

// TODO Game needs to be inserted as a parameter
void World::useCoin() { // NOLINT(*-convert-member-functions-to-static)
    // Hiker hiker = this->getHiker();
    //  TODO update coin score of game
    std::cout << "used coin" << std::endl;
}

void World::useDuck() { // NOLINT(*-convert-member-functions-to-static)
    // Hiker hiker = this->getHiker();
    //  TODO play sounds
    std::cout << "used duck" << std::endl;
}

float World::getMinX() const { return minX; }
void World::setMinX(float minX) { World::minX = minX; }
