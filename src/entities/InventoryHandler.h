//
// Created by Anietta Weckauff on 18.05.24.
//

#ifndef SURVIVING_SARNTAL_INVENTORYHANDLER_H
#define SURVIVING_SARNTAL_INVENTORYHANDLER_H

#include "../input/events/GameEvent.h"
#include "World.h"

/**
 * This struct represents an item that can be collected by the player.
 */
struct CollectableItem {
    ItemType itemType = NO_ITEM;
    float distance = std::numeric_limits<float>::infinity();
};

class InventoryHandler {
  public:
    InventoryHandler();
    /**
     * This method checks whether the hiker can pick up an item.
     * All items currently displayed are checked.
     * @param world
     */
    static void checkCanCollect(World &world);

    /**
     * This method updates the inventory based on the user input.
     * @param gameEvent
     * @param hiker
     */
    static void updateInventory(GameEvent &gameEvent, World &world);

  private:
    static CollectableItem collectableItem;
};

#endif // SURVIVING_SARNTAL_INVENTORYHANDLER_H
