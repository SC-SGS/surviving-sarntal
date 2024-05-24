//
// Created by Anietta Weckauff on 06.05.24.
//
#include "World.h"
#include "Hiker.h"

// World::World() {}

void World::initilizeWorld() {}

void World::setHiker(Hiker &newHiker) { hiker = newHiker; }
Hiker World::getHiker() const { return hiker; }

void World::setInventory(const Inventory &newInventory) { inventory = newInventory; }
Inventory World::getInventory() const { return inventory; }
void World::setMonster(const Monster &newMonster) { monster = newMonster; }
Monster World::getMonster() const { return monster; }
void World::setMountain(const MountainClass &newMountain) { mountain = newMountain; }
MountainClass World::getMountain() const { return mountain; }

void World::addRock(const RockClass &rock) { rocks.push_back(rock); }
void World::setRocks(const std::list<RockClass> &newRocks) { rocks = newRocks; }
std::list<RockClass> World::getRocks() const { return rocks; }

void World::setItems(const std::list<Item> &items) { this->items = items; }
std::list<Item> World::getItems() const { return items; }
void World::addItem(const Item &item) { items.push_back(item); }

void World::useItem(ItemType itemType) const {
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
    Hiker hiker = this->getHiker();
    int currentHealth = hiker.getHealthPoints();
    hiker.setHealthPoints(std::min(currentHealth + KAISERSCHMARRN_HEALTH_RESTORATION, HIKER_MAX_HEALTH));
    std::cout << "used Kaiserschmarrn" << std::endl;
}

// TODO Game needs to be inserted as a parameter
void World::useCoin() const {
    Hiker hiker = this->getHiker();
    // TODO update coin score of game
    std::cout << "used coin" << std::endl;
}
void World::useDuck() const {
    Hiker hiker = this->getHiker();
    // TODO play sounds
    std::cout << "used duck" << std::endl;
}
