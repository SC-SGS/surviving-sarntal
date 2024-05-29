//
// Created by Anietta Weckauff on 18.05.24.
//

#include "../InventoryHandler.h"

CollectableItem InventoryHandler::collectableItem = CollectableItem();

void InventoryHandler::checkCanCollect(World &world) {
    std::list<Item> items = world.getItems();
    Hiker hiker = world.getHiker();
    for (Item item : items) {
        float distance = item.getPosition().distanceTo(hiker.getPosition());
        if (distance < HIKER_ITEM_COLLECTION_RANGE) {
            if (distance < collectableItem.distance) {
                collectableItem.itemType = item.getItemType();
                collectableItem.distance = distance;
            }
        }
    }
}

void InventoryHandler::updateInventory(const GameEvent &gameEvent, World &world) {
    const EventType eventType = gameEvent.type;
    Inventory inventory = world.getInventory();
    switch (eventType) {
    case ITEM_USE:
        if (inventory.getSelectedItem() != ItemType::NO_ITEM) {
            const ItemType selectedItemType = inventory.getSelectedItem();
            world.useItem(selectedItemType);
            if (Item::getItemInformation(selectedItemType).dropOnUse) {
                inventory.drop();
            }
        }
        break;

    case ITEM_DROP:
        if (inventory.getSelectedItem() != ItemType::NO_ITEM) {
            inventory.drop();
        }
        break;
    case AXIS_MODIFICATION:
        if (gameEvent.axis == ITEM_SWITCH) {
            // TODO find out how switch to the left or right
            inventory.switchItem(1);
        }
        break;
    default:
        break;
    }

    if (collectableItem.itemType != NO_ITEM) {
        ItemInformation information = Item::getItemInformation(collectableItem.itemType);
        if ((eventType == ITEM_PICK || information.autoCollect) &&
            collectableItem.distance < 0.3 * HIKER_ITEM_COLLECTION_RANGE) {
            std::cout << "Picking up item: " << collectableItem.itemType << std::endl;
            // TODO play pickup sound
            if (information.useOnPickup) {
                world.useItem(collectableItem.itemType);
            } else {
                if (inventory.getSelectedItem() != NO_ITEM) {
                    inventory.drop();
                }
                inventory.pickup(collectableItem.itemType);
            }
            collectableItem = CollectableItem();
        }
    }
}
