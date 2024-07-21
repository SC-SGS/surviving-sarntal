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
class Hiker;

class World {

  public:
    World(Mountain &mountain, Hiker &hiker, Inventory &inventory, Monster &monster, AudioService &audioService,
          GameConstants gameConstants);
    ~World();
    Hiker &getHiker() const;

    Inventory &getInventory() const;

    Monster &getMonster() const;

    Mountain &getMountain() const;

    std::list<Rock> &getRocks() const;

    int getCoinScore() const;

    int getGameScore() const;

    void updateGameScore();

    /**
     * This method adds a rock to the game by adding it to the list of rocks.
     * @param rock
     */
    void addRock(const Rock &rock) const;

    void addDestroyedRock(Vector position, floatType radius) const;

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

    floatType getMinX() const;
    void setMinX(floatType newMinX);
    floatType getMaxX() const;
    void setMaxX(floatType maxX);

    bool isOutOfScope(const RenderedEntity &entity) const;

  private:
    // Dependencies
    Mountain &mountain;
    Hiker &hiker;
    Inventory &inventory;
    Monster &monster;
    AudioService &audioService;
    GameConstants gameConstants;

    // Attributes
    floatType minX = 0;
    floatType maxX = graphics::SCREEN_WIDTH;

    int coinScore = 0;
    int gameScore = 0;

    const std::unique_ptr<std::list<Rock>> rocks = std::make_unique<std::list<Rock>>();
    const std::unique_ptr<std::list<Rock>> destroyedRocks = std::make_unique<std::list<Rock>>();
    const std::unique_ptr<std::list<std::shared_ptr<Item>>> items =
        std::make_unique<std::list<std::shared_ptr<Item>>>();
};

#endif // SURVIVING_SARNTAL_WORLD_H
