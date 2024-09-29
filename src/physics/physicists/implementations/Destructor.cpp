//
// Created by felix-roehr on 5/27/24.
//

#include "../Destructor.hpp"
#include "../../../output/graphics/Renderer.h"

Destructor::Destructor(World &world, EntityRenderer &entityRenderer, GameConstants &gameConstants)
    : world(world), entityRenderer(entityRenderer), gameConstants(gameConstants) {}

void Destructor::destruct() const {
    destructRocks();
    destructItems();
    destructTerrain();
    destructHiker();
}

void Destructor::destructRocks() const {
    this->world.getRocks().remove_if([this](const std::shared_ptr<Rock> &rock) {
        bool shouldBeDestroyed = rock->getShouldBeDestroyed();
        if (shouldBeDestroyed) {
            // TODO we can make the explosion fit the bounding box perfectly
            this->entityRenderer.addExplosion(*rock);
        }
        return shouldBeDestroyed || this->world.isOutOfScope(*rock);
    });
}

void Destructor::destructItems() const {
    this->world.getItems().remove_if(
        [this](const std::shared_ptr<Item> &item) { return this->world.isOutOfScope(*item); });
}

void Destructor::destructTerrain() const {
    this->world.getTerrain().removeBiome(this->world.getMinX() - this->gameConstants.terrainConstants.bufferLeft);
    // const floatType leftBufferedWidth = this->world.getMountain().getLeftBorder() +
    //                                     (static_cast<floatType>(gameConstants.mountainConstants.chunkBufferLeft) *
    //                                      gameConstants.mountainConstants.chunkWidth);
    // if (this->world.getMinX() > leftBufferedWidth) {
    //     this->world.getMountain().deleteLeftChunk();
    // }
}

void Destructor::destructHiker() const {
    const bool needsToDie = this->world.getHiker().getPosition().x - this->world.getMonster().getXPosition() <= 0;
    const bool isAlive = this->world.getHiker().getHealthPoints() > 0;
    if (needsToDie || !isAlive) {
        this->world.getHiker().kill();
    }
}