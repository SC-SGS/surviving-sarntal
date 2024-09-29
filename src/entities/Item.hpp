//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ITEM_HPP
#define SURVIVING_SARNTAL_ITEM_HPP

#include "../output/graphics/renderInformation/RenderInformation.h"
#include "../utilities/vector.h"
#include "RenderedEntity.h"
#include "string"

/**
 * Enum to specify the different types of items that occur in the game.
 */
enum ItemType { NO_ITEM = -1, KAISERSCHMARRN = 0, COIN = 1, DUCK_ITEM = 2, ROCK_BOMB = 3 };

struct ItemDto {
    std::string name;
    ItemType itemType;
    bool autoCollect;
    bool useOnPickup;
    bool dropOnUse;
    int spawnWeight;
};

/**
 * This class represents an Item in the game. An item can be of type
 * Kaiserschmarrn, Coin or Duck.
 * Items can be used by the player and have a positive effect on the game
 * and player state.
 */
class Item : public RenderedEntity {
  public:
    Item(ItemType itemType, Vector position, floatType baseHeight, const ItemDto &dto);

    RenderInformation getRenderInformation() const override;

    ItemType getItemType() const;

    std::string getName() const;

    bool canAutoCollect() const;

    bool canUseOnPickUp() const;

    bool shouldDropOnUse() const;

  private:
    floatType baseHeight;
    ItemType itemType;
    std::string name;
    std::string audio;
    bool autoCollect = false;
    bool useOnPickup = false;
    bool dropOnUse = false;
    int spawnWeight = 0;

    void buildItem(const ItemDto &dto);
};

#endif // SURVIVING_SARNTAL_ITEM_HPP
