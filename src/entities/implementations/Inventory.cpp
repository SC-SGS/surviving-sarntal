//
// Created by Anietta Weckauff on 04.05.24.
//

#include "../Inventory.hpp"

#include "../../output/audio/AudioService.hpp"
#include "spdlog/spdlog.h"
#include <iostream>

Inventory::Inventory(size_t slotCount, AudioService &audioService, ItemsConstants itemsConstants)
    : slots(slotCount), audioService(audioService), itemsConstants(itemsConstants) {
    for (size_t i = 0; i < slotCount; ++i) {
        slots[i] = std::vector<std::shared_ptr<Item>>();
    }
    spdlog::info("Inventory initialized.");
}

Inventory::Inventory(AudioService &audioService, ItemsConstants itemsConstants)
    : Inventory(itemsConstants.slotsPerInventory, audioService, itemsConstants) {}

size_t Inventory::getNumberOfSlots() const { return slots.size(); }

void Inventory::addItem(const std::shared_ptr<Item> &item) {
    const bool itemInInventory = itemTypeInInventory(item->getItemType());

    if (itemInInventory) {
        int slot = getSlotOfItem(item->getItemType());
        addItem(slot, item);
    } else if (itemSlotAvailable()) {
        int nextFreeSlot = getNextFreeSlot();
        addItem(nextFreeSlot, item);
    }
}

void Inventory::removeSelectedItem() {
    if (!slots[selectedSlot].empty()) {
        slots[selectedSlot].pop_back();
    }
}

ItemType Inventory::getItemType(size_t slotNumber) const {
    const bool validSlot = slotNumber < slots.size();
    if (validSlot) {
        if (!slots[slotNumber].empty()) {
            return slots[slotNumber].front()->getItemType();
        }
    }
    return ItemType::NO_ITEM;
}

ItemType Inventory::getSelectedItemType() const { return getItemType(selectedSlot); }

size_t Inventory::getSelectedSlot() const { return selectedSlot; }

void Inventory::switchItemSlot(int offset) { selectedSlot = (selectedSlot + offset) % getNumberOfSlots(); }

bool Inventory::selectedSlotIsEmpty() const { return slots[selectedSlot].empty(); }

int Inventory::getSlotOfItem(ItemType itemType) {
    int slotNumber = -1;
    for (int i = 0; i < slots.size(); i++) {
        if (!slots[i].empty()) {
            if (slots[i].front()->getItemType() == itemType) {
                slotNumber = i;
            }
        }
    }
    return slotNumber;
}

bool Inventory::itemTypeInInventory(ItemType itemType) { return getSlotOfItem(itemType) != -1; }

bool Inventory::itemSlotAvailable() {
    return std::any_of(slots.begin(), slots.end(),
                       [](const std::vector<std::shared_ptr<Item>> &slot) { return slot.empty(); });
}

int Inventory::getNextFreeSlot() {
    for (int i = 0; i < slots.size(); i++) {
        if (slots[i].empty()) {
            return i;
        }
    }
    return -1;
}

void Inventory::addItem(int slot, const std::shared_ptr<Item> &item) {
    if (slots[slot].size() < itemsConstants.itemsPerSlot) {
        slots[slot].push_back(item);
        this->audioService.playSound("pickup-item");
    }
}

bool Inventory::canCollectItem(const std::shared_ptr<Item> &item) {
    if (itemTypeInInventory(item->getItemType())) {
        const int slot = getSlotOfItem(item->getItemType());
        return slots[slot].size() < itemsConstants.itemsPerSlot;
    } else {
        return getNextFreeSlot() != -1;
    }
}

size_t Inventory::getNumberOfItems(size_t slot) const {
    if (slot < slots.size()) {
        return slots[slot].size();
    }
    return 0;
}

std::shared_ptr<Item> Inventory::getSelectedItem() const {
    if (!this->selectedSlotIsEmpty()) {
        return this->slots[selectedSlot].front();
    } else {
        spdlog::critical("The selected slot is emtpy and no item could be retrieved.");
        throw std::runtime_error("The selected slot is emtpy and no item could be retrieved.");
    }
}
bool Inventory::slotIsEmpty(size_t slot) const {
    if (slot < slots.size()) {
        return slots[slot].empty();
    } else {
        spdlog::critical("The slot {} does not exist in the inventory.", slot);
        throw std::runtime_error("The slot does not exist.");
    }
}
std::shared_ptr<Item> Inventory::getItem(size_t slot) const {
    const bool canGetItem = !slotIsEmpty(slot) && slot < slots.size();
    if (canGetItem) {
        return this->slots[slot].front();
    } else {
        spdlog::critical("No Item could be retrieved from slot: {}.", slot);
        throw std::runtime_error("The slot is emtpy or the slot number is too high and no item could be retrieved.");
    }
}
