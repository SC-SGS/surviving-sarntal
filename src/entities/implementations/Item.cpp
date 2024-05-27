//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Item.h"
#include <iostream>
#include <utility>

Item::Item(ItemType itemType, ItemInformation itemInformation, Vector position)
    : RenderedEntity(position), itemID(itemType), itemInformation(std::move(itemInformation)) {}

ItemInformation Item::getItemInformation(ItemType itemType) {
    switch (itemType) {
    case KAISERSCHMARRN:
        return {"Kaiserschmarrn", false, false, true};
    case COIN:
        return {"Coin", false, true, true};
    case DUCK_ITEM:
        return {"Duck", true, false, true};
    default:
        return {"NoItem", false, false, false};
    }
}

ItemType Item::getItemType() { return this->itemID; }
