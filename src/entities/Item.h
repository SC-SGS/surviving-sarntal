//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ITEM_H
#define SURVIVING_SARNTAL_ITEM_H

#include "../components/vector.h"
#include "Hiker.h"
#include "string"

/**
 * Enum to specify the different types of items that occur in the game.
 */
enum ItemType { NO_ITEM = -1, KAISERSCHMARRN = 0, COIN = 1, DUCK = 2 };

/**
 * This class represents an Item in the game. An item can be of type
 * Kaiserschmarrn, Coin or Duck.
 * Items can be used by the player and have a positive effect on the game
 * and player state.
 */
class Item : public RenderedEntity {
  public:
    Item();
    Item(ItemType itemType, std::string name, std::string texture,
         std::string audio, bool autoCollect, bool useOnPickup, bool dropOnUse);

    // world needs to be inserted as an argument
    static void useItem(ItemType itemType, Hiker &hiker);

  private:
    ItemType itemID;
    std::string name;
    std::string texture;
    std::string audio;
    bool autoCollect;
    bool useOnPickup;
    bool dropOnUse;
    static void useCoin();
    static void useDuck();
    static void useKaiserschmarrn(Hiker &hiker);
    // float interactionRadius;
};

#endif // SURVIVING_SARNTAL_ITEM_H
