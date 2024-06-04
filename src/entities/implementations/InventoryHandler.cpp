//
// Created by Anietta Weckauff on 18.05.24.
//

#include "../InventoryHandler.h"
#include <iostream>

#include <mutex>

InventoryHandler::InventoryHandler() { std::cout << "InventoryHandler initialized." << std::endl; }

InventoryHandler::~InventoryHandler() { std::cout << "InventoryHandler destroyed." << std::endl; }

void InventoryHandler::pickUpItem(const std::shared_ptr<Item> &item) {
    Inventory &inventory = this->world.getInventory();
    int counter = 0;
    while (counter < inventory.getSlotCount()) {
        if (inventory.isSelectedSlotFree()) {
            inventory.pickup(item->getItemType());
            return;
        }
        inventory.switchItem(1);
        counter++;
    }
    inventory.pickup(item->getItemType());
    this->world.getItems().remove(item);
}

void InventoryHandler::removeSelectedItem() {
    Inventory &inventory = this->world.getInventory();
    inventory.drop();
}
