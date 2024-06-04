//
// Created by Anietta Weckauff on 04.05.24.
//

#ifndef SURVIVING_SARNTAL_INVENTORY_HPP
#define SURVIVING_SARNTAL_INVENTORY_HPP

#include "Item.hpp"
#include "string"
#include "vector"

struct ItemSlot {
    ItemType itemType = ItemType::NO_ITEM;
};

/**
 * This class represents the inventory of a player. An inventory stores several
 * items.
 * TODO we need to refactor the whole thing with the items. this is not loosely coupled and very weird an unintuitive
 * TODO inventory should have a list of references to item objects
 */
class Inventory {

  private:
    std::vector<ItemSlot> slots;
    size_t selectedSlot = 0;

  public:
    explicit Inventory(size_t slotCount);
    Inventory();

    static void initItems();

    /**
     * @brief This method returns the number of slots in the inventory.
     * @return number of slots
     */
    size_t getSlotCount() const;

    /**
     * @brief This method adds a picked-up item to the inventory.
     * @param itemType
     */
    void pickup(ItemType itemType);

    /**
     * @brief This method clear the selected slot of the inventory.
     */
    void drop();

    /**
     * @brief This method sets the given item type for a given slot.
     * @param slotNumber
     * @param itemType
     */
    void setItem(size_t slotNumber, ItemType itemType);

    /**
     * @brief This method returns the item type at a given slot number.
     * @param slotNumber
     * @return item type at the given slot
     */
    ItemType getItem(size_t slotNumber) const;

    /**
     * @brief This method returns the item type at the currently selected slot.
     * @return currently selected item type
     */
    ItemType getSelectedItem() const;

    /**
     * @brief This method returns the index of the currently selected slot.
     * @return current slot number
     */
    size_t getSelectedSlot() const;

    /**
     * returns whether the selected slot has an item.
     */
    bool isSelectedSlotFree() const;

    /**
     * switches the selected slot by the given offset (+i to switch i slots
     * forward, -i backward)
     * @param offset
     */
    void switchItem(int offset);
};

#endif // SURVIVING_SARNTAL_INVENTORY_HPP
