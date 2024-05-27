//
// Created by Anietta Weckauff on 06.05.24.
//

#ifndef SURVIVING_SARNTAL_WORLD_H
#define SURVIVING_SARNTAL_WORLD_H

#include "Hiker.h"
#include "Inventory.h"
#include "Item.h"
#include "Monster.h"
#include "Mountain.h"
#include "Rock.h"
#include <iostream>
#include <list>

/**
 * This class represents our game world. It owns all the entities that occur in our game world,
 * including the hiker, monster, mountain as well as a list of rocks and a list of items.
 * This class serves the purpose to combine all the single entities into one structure.
 */

class World {

  public:
    World();
    void initializeWorld();

    void setHiker(Hiker &newHiker);
    Hiker getHiker() const;

    void setInventory(const Inventory &newInventory);
    Inventory getInventory() const;

    void setMonster(const Monster &newMonster);
    Monster getMonster() const;

    void setMountain(const MountainClass &newMountain);
    MountainClass getMountain() const;

    void setRocks(const std::list<RockClass> &newRocks);
    std::list<RockClass> getRocks() const;

    /**
     * This method adds a rock to the game by adding it to the list of rocks.
     * @param rock
     */
    void addRock(const RockClass &rock);

    void setItems(const std::list<Item> &items);
    std::list<Item> getItems() const;

    /**
     * ´This method adds an item to the game by adding it to the list of items
     * @param item
     */
    void addItem(const Item &item);

    /**
     * This method uses an item, performing the respective change to player and
     * game state.
     * @param itemType
     */
    void useItem(ItemType itemType) const;

    void useCoin() const;
    void useDuck() const;
    void useKaiserschmarrn() const;

  private:
    Hiker hiker;
    Inventory inventory;
    Monster monster;
    MountainClass mountain;
    std::list<RockClass> rocks;
    std::list<Item> items;
};

#endif // SURVIVING_SARNTAL_WORLD_H
