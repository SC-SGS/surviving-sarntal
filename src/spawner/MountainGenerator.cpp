//
// Created by Anietta Weckauff on 13.05.24.
//
#include "MountainGenerator.h"
#include <iostream>
#include <mutex>

MountainGenerator::MountainGenerator() { MountainGenerator::loadChunksOfMountain(); }

MountainGenerator::~MountainGenerator() = default;

void MountainGenerator::generateMountainChunk() {
    const floatType currentLeftEdgeScreen{this->world.getMinX()};
    const floatType leftestPointOfMountain{
        this->world.getMountain().getVertex(this->world.getMountain().getIndexIntervalOfEntireMountain().startIndex).x};
    constexpr floatType CHUNK_DESTROY_BUFFER_CONSTANT{1.0};

    if (leftestPointOfMountain < currentLeftEdgeScreen - Mountain::CHUNK_WIDTH - CHUNK_DESTROY_BUFFER_CONSTANT) {

        if (this->world.getHiker().getIsAlive()) {
            this->world.getMountain().generateNewChunk();
            spdlog::debug("Mountain chunk generated.");
        }
    }
}
void MountainGenerator::loadChunksOfMountain() {
    Mountain &mountain = Mountain::getInstance();

    for (std::size_t i{0}; i < Mountain::NUMBER_OF_VERTICES / Mountain::NUM_SECTIONS_PER_CHUNK; i++) {
        mountain.generateNewChunk();
    }
    spdlog::debug("Loaded chunks of mountain,");
}
