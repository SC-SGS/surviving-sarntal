//
// Created by felix-roehr on 5/27/24.
//

#include "../Destructor.hpp"
#include "../../../output/graphics/Renderer.h"

Destructor::Destructor(World &world, Renderer &renderer) : world(world), renderer(renderer) {}

void Destructor::destruct() const {
    destructRocks();
    destructItems();
    destructMountain();
    destructHiker();
}

void Destructor::destructRocks() const {
    this->world.getRocks().remove_if([this](const Rock &rock) {
        bool shouldBeDestroyed = rock.getShouldBeDestroyed();
        if (shouldBeDestroyed) {
            this->renderer.addExplosion(rock.getPosition(), rock.getRadius());
        }
        return shouldBeDestroyed || this->world.isOutOfScope(rock);
    });
}

void Destructor::destructItems() const {
    this->world.getItems().remove_if(
        [this](const std::shared_ptr<Item> &item) { return this->world.isOutOfScope(*item); });
}

void Destructor::destructMountain() const {
    // TODO: Implement when mountain implementation is reworked
}

void Destructor::destructHiker() const {
    const bool needsToDie = this->world.getHiker().getPosition().x - this->world.getMonster().getXPosition() <= 0;
    const bool isAlive = this->world.getHiker().getHealthPoints() > 0;
    if (needsToDie || !isAlive) {
        this->world.getHiker().kill();
    }
}