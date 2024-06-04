//
// Created by Anietta Weckauff on 06.05.24.
//

#ifndef SURVIVING_SARNTAL_WORLD_H
#define SURVIVING_SARNTAL_WORLD_H

#include "../utilities/Singleton.hpp"
#include "Hiker.h"
#include "Inventory.hpp"
#include "Item.hpp"
#include "Monster.h"
#include "Mountain.h"
#include "Rock.h"

#include <list>
#include <memory>

/**
 * This class represents our game world. It owns all the entities that occur in our game world,
 * including the hiker, monster, mountain as well as a list of rocks and a list of items.
 * This class serves the purpose to combine all the single entities into one structure.
 */

// TODO Should be singleton (see Physics Engine for implementation details)
// TODO I dislike that our getters are non-const. It makes the design kinda weird.
class World : public Singleton<World> {
    friend class Singleton<World>; // Allow Singleton to access the constructor??

  public:
    // TODO it would be nice to have getters and mutable getters. There should be an immutable getter option (const).

    Hiker &getHiker() const;

    Inventory &getInventory() const;

    Monster &getMonster() const;

    MountainClass &getMountain() const;

    std::list<RockClass> &getRocks();

    /**
     * This method adds a rock to the game by adding it to the list of rocks.
     * @param rock
     */
    void addRock(RockClass &rock);

    std::list<std::shared_ptr<Item>> &getItems() const;

    /**
     * ´This method adds an item to the game by adding it to the list of items
     * @param item
     */
    void addItem(const Item &item) const;

    /**
     * This method uses an item, performing the respective change to player and
     * game state.
     * @param itemType
     * TODO this belongs in item
     */
    void useItem(ItemType itemType);

    void useCoin();
    void useDuck();
    void useKaiserschmarrn() const;

    /**
     * Returns a list of all items that are close enough to the hiker to be picked up
     *
     * @return list of nearby items
     */
    std::list<std::shared_ptr<Item>> getNearbyItems() const;

    float getMinX() const;
    void setMinX(float minX);
    float getMaxX() const;
    void setMaxX(float maxX);

    bool isOutOfScope(RenderedEntity &entity) const;

  private:
    // TODO into yaml config and check value
    // constexpr Vector DEFAULT_HIKER_POS = {0, MountainClass::getInstance().getYPosFromX(0)};
    static constexpr size_t DEFAULT_INV_SLOT_NUM = 3;

    // TODO we need to make sure that these attributes are only saved here, i.e. there are only pointers everywhere else
    // TODO check hiker and/or inventory singleton?
    MountainClass &mountain = MountainClass::getInstance();
    float hikerPositionX = 0.8 * graphics::SCREEN_WIDTH;
    std::unique_ptr<Hiker> hiker =
        std::make_unique<Hiker>(Vector{hikerPositionX, mountain.getYPosFromX(hikerPositionX)});
    // TODO inventory belongs to HIKER!!!
    std::unique_ptr<Inventory> inventory = std::make_unique<Inventory>(DEFAULT_INV_SLOT_NUM);
    Monster &monster = Monster::getInstance();

    float minX = 0;
    float maxX = graphics::SCREEN_WIDTH;

    int coinScore = 0;

    const std::unique_ptr<std::list<RockClass>> rocks = std::make_unique<std::list<RockClass>>();
    const std::unique_ptr<std::list<std::shared_ptr<Item>>> items =
        std::make_unique<std::list<std::shared_ptr<Item>>>();
    World();
    ~World();

    // TODO spawn rock method and destruct rock method or at least add and remove from list
};

#endif // SURVIVING_SARNTAL_WORLD_H
