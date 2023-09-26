#ifndef SURVIVING_SARNTAL_INVENTORY_H
#define SURVIVING_SARNTAL_INVENTORY_H

#include "flecs.h"
#include "string"
#include <array>
#include <list>
#include <raylib.h>
#include <unordered_map>
#include <vector>

struct Item {
    std::string name;
};

std::array<Item, 1> items{{"TestItem"}};

constexpr int NO_ITEM = -1;

struct ItemSlot {
    int item_id = NO_ITEM;
};

class Inventory {
  private:
    std::vector<ItemSlot> slots;
    size_t selected_slot = 0;

    size_t slotCount() { return slots.size(); }

  public:
    void pickup(int item_id) { slots[selected_slot].item_id = item_id; }
    void drop() { slots[selected_slot].item_id = NO_ITEM; }

    void use() {
        // TODO
    }

    void switchItem(int offset) {
        selected_slot = (selected_slot + offset + slotCount()) % slotCount();
    }
};
#endif
