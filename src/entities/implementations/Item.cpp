//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Item.hpp"

Item::Item(ItemType itemType, Vector position) : RenderedEntity(position), itemType(itemType) { this->buildItem(); }

ItemType Item::getItemType() const { return this->itemType; }

RenderInformation Item::getRenderInformation() {
    return RenderInformation{Vector2(this->position), {0, 0}, ITEM_BASE_HEIGHT, ITEM_BASE_HEIGHT, 0, this->name};
}

void Item::buildItem() {
    switch (this->itemType) {
    case KAISERSCHMARRN:
        this->name = "kaiserschmarrn";
        break;
    case COIN:
        this->name = "coin";
        break;
    case DUCK_ITEM:
        this->name = "duck";
        break;
    default:
        this->name = "coin";
    }

    YAML::Node item = ConfigManager::getInstance().getItems()[this->name];
    this->autoCollect = item["autoCollect"].as<bool>();
    this->useOnPickup = item["useOnPickUp"].as<bool>();
    this->dropOnUse = item["dropOnUse"].as<bool>();
}

std::string Item::getName() const { return this->name; }
bool Item::canAutoCollect() const { return this->autoCollect; }
bool Item::canUseOnPickUp() const { return this->useOnPickup; }
bool Item::shouldDropOnUse() const { return this->dropOnUse; }
