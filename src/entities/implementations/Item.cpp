//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Item.hpp"
#include <iostream>
#include <utility>

// TODO why is item information always copied?
Item::Item(const ItemType itemType, ItemInformation itemInformation, const Vector position)
    : RenderedEntity(position), itemID(itemType), itemInformation(std::move(itemInformation)) {}

ItemInformation Item::getItemInformation(const ItemType type) {
    switch (type) {
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

ItemType Item::getItemType() const { return this->itemID; }
