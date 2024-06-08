//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Item.hpp"

Item::Item(ItemType itemType, Vector position) : RenderedEntity(position), itemType(itemType) { this->buildItem(); }

ItemType Item::getItemType() const { return this->itemType; }

RenderInformation Item::getRenderInformation() {
    return RenderInformation{Vector2(this->position), {0, 0}, ITEM_BASE_HEIGHT, ITEM_BASE_HEIGHT, {0, 0}, this->name};
}

// TODO make better
void Item::buildItem() {
    switch (this->itemType) {
    case KAISERSCHMARRN:
        this->name = "Kaiserschmarrn";
        this->autoCollect = false;
        this->useOnPickup = false;
        this->dropOnUse = true;
        break;
    case COIN:
        this->name = "Coin";
        this->autoCollect = true;
        this->useOnPickup = false;
        this->dropOnUse = true;
        break;
    case DUCK_ITEM:
        this->name = "Duck";
        this->autoCollect = true;
        this->useOnPickup = false;
        this->dropOnUse = true;
        break;
    default:
        this->name = "NoItem"; // todo remove ?? Add texture for Noitem
        this->autoCollect = false;
        this->useOnPickup = false;
        this->dropOnUse = false;
        break;
    }
}
std::string Item::getName() const { return this->name; }
bool Item::canAutoCollect() const { return this->autoCollect; }
bool Item::canUseOnPickUp() const { return this->useOnPickup; }
bool Item::shouldDropOnUse() const { return this->dropOnUse; }
