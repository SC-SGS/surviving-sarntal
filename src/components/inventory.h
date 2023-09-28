#ifndef SURVIVING_SARNTAL_INVENTORY_H
#define SURVIVING_SARNTAL_INVENTORY_H

#include "flecs.h"
#include "input.h"
#include "list"
#include "player.h"
#include "string"
#include "vector.h"
#include <array>
#include <iostream>
#include <list>
#include <raylib.h>
#include <unordered_map>
#include <vector>

struct ItemClass {
    std::string name;
    std::string texture;
    std::string audio;
    bool auto_collect;
    bool use_on_pickup;
    bool drop_on_use;
    void (*use)(const flecs::world &world, flecs::entity &player);

    enum Items {
        NO_ITEM = -1,
        KAISERSCHMARRN = 0, // lecker
        COIN = 1,
        DUCK = 2,
        ITEM_COUNT
    };

    static void useKaiserschmarrn(const flecs::world &world,
                                  flecs::entity &player);
    static void useCoin(const flecs::world &world, flecs::entity &player);
    static void useDuck(const flecs::world &world, flecs::entity &player);
};

// define item types here
const std::vector<ItemClass> ITEM_CLASSES{
    {"Kaiserschmarrn", "../assets/texture/kaiserschmarrn.png",
     "../assets/audio/kaiserschmarrn.mp3", false, true, true,
     ItemClass::useKaiserschmarrn},
    {"Coin", "../assets/texture/raylib_256x256.png", "../assets/audio/coin.mp3",
     false, true, true, ItemClass::useCoin},
    {"Duck", "../assets/texture/duck.png", "../assets/audio/duck.mp3", true,
     false, true, ItemClass::useDuck}};

struct Item {
    ItemClass::Items item_id;
};

struct InteractionRadius {
    float radius;
};

struct CanCollect {
    flecs::entity item_entity;
    float distance = 0;
};

struct ItemSlot {
    ItemClass::Items item_type = ItemClass::Items::NO_ITEM;
};

class Inventory {
  public:
    static void checkCanCollect(flecs::iter it, Position *positions,
                                InteractionRadius *radii);
    static void updateInventory(flecs::iter it, InputEntity *input_entities,
                                Inventory *inventory_entities);

  private:
    std::vector<ItemSlot> slots;
    size_t selected_slot = 0;

  public:
    Inventory(size_t slot_count);
    Inventory();

    size_t getSlotCount() const;

    void pickup(ItemClass::Items item_type);
    void drop();

    void consume();
    void setItem(size_t slot, ItemClass::Items item_type);
    ItemClass::Items getItem(size_t slot) const;
    ItemClass::Items getSelectedItem() const;

    size_t getSelectedSlot() const;

    /**
     * switches the selected slot by the given offset (+i to switch i slots
     * forward, -i backward)
     * @param offset
     */
    void switchItem(int offset);
};
#endif
