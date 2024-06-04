//
// Created by Anietta Weckauff on 18.05.24.
//

#ifndef SURVIVING_SARNTAL_INVENTORYHANDLER_H
#define SURVIVING_SARNTAL_INVENTORYHANDLER_H

#include "../input/events/GameEvent.h"
#include "../utilities/Singleton.hpp"
#include "World.h"

class InventoryHandler : public Singleton<InventoryHandler> {
    friend class Singleton<InventoryHandler>;

  public:
    /**
     * This method inserts a new item into the inventory. If the currently selected slot is free,
     * the item will be inserted into the inventory. Otherwise, the next free slot is searched.
     * If there is no free slot, the currently selected slot is overwritten.
     * When the item is picked up, it is removed from the world.
     * @param item
     */
    void pickUpItem(const std::shared_ptr<Item> &item);

    /**
     * This method removes the currently selected item from the inventory when it is
     * used by the player.
     */
    void removeSelectedItem();

  private:
    World &world = World::getInstance();
    InventoryHandler();
    ~InventoryHandler();
};

#endif // SURVIVING_SARNTAL_INVENTORYHANDLER_H
