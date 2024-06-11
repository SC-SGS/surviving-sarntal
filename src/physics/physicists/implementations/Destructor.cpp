//
// Created by felix-roehr on 5/27/24.
//

#include "../Destructor.hpp"

#include "../../../entities/Item.hpp"

#include <iostream>
#include <mutex>

Destructor::Destructor() : world(World::getInstance()) {}

Destructor::~Destructor() = default;

void Destructor::destruct() const {
    destructRocks();
    destructItems();
    destructMountain();
}

void Destructor::destructRocks() const {
    this->world.getRocks().remove_if(
        [this](Rock rock) { return this->world.isOutOfScope(rock) || rock.getShouldBeDestroyed(); });
}

void Destructor::destructItems() const {
    this->world.getItems().remove_if(
        [this](const std::shared_ptr<Item> &item) { return this->world.isOutOfScope(*item); });
}

void Destructor::destructMountain() const {
    // TODO: Implement when mountain implementation is reworked
}