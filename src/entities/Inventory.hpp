//
// Created by Anietta Weckauff on 04.05.24.
//

#ifndef SURVIVING_SARNTAL_INVENTORY_HPP
#define SURVIVING_SARNTAL_INVENTORY_HPP

#include "../output/audio/AudioService.hpp"
#include "Item.hpp"
#include "string"
#include "vector"
#include <algorithm>
#include <memory>

/**
 * This class represents the inventory of a player. An inventory stores several
 * items.
 */
class Inventory {

  private:
    std::vector<std::vector<std::shared_ptr<Item>>> slots;
    size_t selectedSlot = 0;
    AudioService &audioService;
    ItemsConstants &itemsConstants;

    bool itemTypeInInventory(ItemType itemType);
    bool itemSlotAvailable();
    int getNextFreeSlot();
    void addItem(int slot, const std::shared_ptr<Item> &item);

  public:
    int getSlotOfItem(ItemType itemType);
    explicit Inventory(size_t slotCount, AudioService &audioService, ItemsConstants &itemsConstants);
    explicit Inventory(AudioService &audioService, ItemsConstants &itemsConstants);

    /**
     * @brief This method returns the number of slots in the inventory.
     * @return number of slots
     */
    size_t getNumberOfSlots() const;

    /**
     * @brief This method adds an item to the inventory.
     *
     * If there are free item slots and the item is added to the next free slot.
     * If the itemType is already in the inventory and there is space left in that slot the item is added to that slot,
     * otherwise the nothing is done with the item.
     *
     * @param item
     */
    void addItem(const std::shared_ptr<Item> &item);

    /**
     * Return true if the item can be collected.
     *
     * Returns true if one of the slots has the same item type and
     * at least one more item of that type can be added to the inventory.
     * Return true if the item type is not yet in the inventory and and there is at least one free inventory slot.
     *
     * @param item
     * @return
     */
    bool canCollectItem(const std::shared_ptr<Item> &item);

    /**
     * @brief This method clear the currently selected slot of the inventory.
     */
    void removeSelectedItem();

    /**
     * @brief This method returns the item type at a given slot number.
     * @param slotNumber
     * @return item type at the given slot
     */
    ItemType getItemType(size_t slotNumber) const;

    /**
     * @brief This method returns the item type at the currently selected slot.
     * @return currently selected item type
     */
    ItemType getSelectedItemType() const;

    /**
     * Returns the Item saved in the current slot. If the slot is empty a runtime exception is thrown.
     * @return current selected Item
     */
    std::shared_ptr<Item> getSelectedItem() const;

    std::shared_ptr<Item> getItem(size_t slot) const;

    /**
     * @brief This method returns the index of the currently selected slot.
     * @return current slot number
     */
    size_t getSelectedSlot() const;

    /**
     * returns whether the selected slot has an item.
     */
    bool selectedSlotIsEmpty() const;

    bool slotIsEmpty(size_t slot) const;

    /**
     * switches the selected slot by the given offset (+i to switch i slots
     * forward, -i backward)
     * @param offset
     */
    void switchItemSlot(int offset);

    size_t getNumberOfItems(size_t slot) const;
    void reset();
};

#endif // SURVIVING_SARNTAL_INVENTORY_HPP
