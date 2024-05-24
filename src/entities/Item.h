//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ITEM_H
#define SURVIVING_SARNTAL_ITEM_H

#include "../components/vector.h"
#include "../graphics/render_information/RenderInformation.h"
#include "RenderedEntity.h"
#include "string"

/**
 * Enum to specify the different types of items that occur in the game.
 */
enum ItemType { NO_ITEM = -1, KAISERSCHMARRN = 0, COIN = 1, DUCK_ITEM = 2 };

struct ItemInformation {
    std::string name;
    bool autoCollect;
    bool useOnPickup;
    bool dropOnUse;
};

/**
 * This class represents an Item in the game. An item can be of type
 * Kaiserschmarrn, Coin or Duck.
 * Items can be used by the player and have a positive effect on the game
 * and player state.
 */
class Item : public RenderedEntity {
  public:
    Item(ItemType itemType, ItemInformation itemInformation, Vector position);

    /**
     * This method gets the information about an item based on the item type.
     * @param itemType
     * @return item information
     */
    static ItemInformation getItemInformation(ItemType itemType);

    ItemType getItemType();

  private:
    ItemType itemID;
    ItemInformation itemInformation;
    std::string name;
    std::string texture;
    std::string audio;
    bool autoCollect{};
    bool useOnPickup{};
    bool dropOnUse{};
};

#endif // SURVIVING_SARNTAL_ITEM_H
