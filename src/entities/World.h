//
// Created by Anietta Weckauff on 06.05.24.
//

#ifndef SURVIVING_SARNTAL_WORLD_H
#define SURVIVING_SARNTAL_WORLD_H

#include "../utils/game_constants.h"
#include "Hiker.h"
#include "Inventory.hpp"
#include "Item.hpp"
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

// TODO Should be singleton (see Physics Engine for implementation details)
// TODO I dislike that our getters are non-const. It makes the design kinda weird.
class World {

  public:
    World() = delete;
    void initializeWorld();
    // TODO it would be nice to have getters and mutable getters. There should be an immutable getter option (const).

    void setHiker(const Hiker &newHiker);
    Hiker &getHiker();

    void setInventory(const Inventory &newInventory);
    Inventory &getInventory();

    void setMonster(const Monster &newMonster);
    Monster &getMonster();

    void setMountain(const MountainClass &newMountain);
    MountainClass &getMountain();

    void setRocks(const std::list<RockClass *> &newRocks);
    std::list<RockClass *> &getRocks();

    /**
     * This method adds a rock to the game by adding it to the list of rocks.
     * @param rock
     */
    void addRock(RockClass *rock);

    void setItems(const std::list<Item> &items);
    std::list<Item> &getItems();

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
    void useItem(ItemType itemType);

    void useCoin();
    void useDuck();
    void useKaiserschmarrn();

    /**
     * Returns a list of all items that are close enough to the hiker to be picked up
     * TODO: implement
     *
     * @return list of nearby items
     */
    std::list<Item> getNearbyItems() const;

  private:
    // TODO we need to make sure that these attributes are only saved here, i.e. there are only pointers everywhere else
    Hiker hiker;
    // TODO why does the world have an inventory and not the hiker?
    Inventory inventory;
    Monster monster;
    MountainClass mountain;
    float minX;

  public:
    float getMinX() const;
    void setMinX(float minX);
    float getMaxX() const;
    void setMaxX(float maxX);

    bool isOutOfScope(RenderedEntity &entity) const;

  private:
    float maxX;

    std::list<RockClass *> rocks;
    std::list<Item> items;

    // TODO spawn rock method and destruct rock method or at least add and remove from list
};

#endif // SURVIVING_SARNTAL_WORLD_H
