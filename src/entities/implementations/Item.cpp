//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Item.hpp"

Item::Item(ItemType itemType, Vector position, floatType baseHeight, const ItemDto &dto)
    : RenderedEntity(position), itemType(itemType), baseHeight(baseHeight) {
    this->buildItem(dto);
}

ItemType Item::getItemType() const { return this->itemType; }

RenderInformation Item::getRenderInformation() const {
    return RenderInformation{Vector2(this->position), {0, 0}, this->baseHeight, this->baseHeight, 0, this->name};
}

void Item::buildItem(const ItemDto &dto) {
    this->name = dto.name;
    this->itemType = dto.itemType;
    this->autoCollect = dto.autoCollect;
    this->useOnPickup = dto.useOnPickup;
    this->dropOnUse = dto.dropOnUse;
    this->spawnWeight = dto.spawnWeight;
}

std::string Item::getName() const { return this->name; }
bool Item::canAutoCollect() const { return this->autoCollect; }
bool Item::canUseOnPickUp() const { return this->useOnPickup; }
bool Item::shouldDropOnUse() const { return this->dropOnUse; }
