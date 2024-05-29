//
// Created by Anietta Weckauff on 04.05.24.
//

#include "../Inventory.hpp"

Inventory::Inventory(size_t slotCount) : slots(slotCount, ItemSlot{ItemType::NO_ITEM}), selectedSlot(0) {}

Inventory::Inventory() : Inventory(3) {}

size_t Inventory::getSlotCount() const { return slots.size(); }

void Inventory::initItems() {}

void Inventory::pickup(ItemType itemType) { slots[selectedSlot].itemType = itemType; }

void Inventory::drop() { slots[selectedSlot].itemType = ItemType::NO_ITEM; }

void Inventory::setItem(size_t slotNumber, ItemType itemType) { slots[slotNumber].itemType = itemType; }

ItemType Inventory::getItem(size_t slotNumber) const { return slots[slotNumber].itemType; }

ItemType Inventory::getSelectedItem() const { return getItem(getSelectedSlot()); }

size_t Inventory::getSelectedSlot() const { return selectedSlot; }

void Inventory::switchItem(int offset) { selectedSlot = (selectedSlot + offset + getSlotCount()) % getSlotCount(); }
