//
// Created by Anietta Weckauff on 13.05.24.
//
#include "MountainGenerator.h"
#include <iostream>
#include <mutex>

MountainGenerator::MountainGenerator() {
    MountainGenerator::loadChunksOfMountain();
    std::cout << "MountainGenerator initialized." << std::endl;
}

MountainGenerator::~MountainGenerator() { std::cout << "MountainGenerator destroyed." << std::endl; }

void MountainGenerator::generateMountainChunk() {
    const float currentLeftEdgeScreen{this->world.getMinX()};
    const float leftestPointOfMountain{
        this->world.getMountain().getVertex(this->world.getMountain().getIndexIntervalOfEntireMountain().startIndex).x};
    constexpr float CHUNK_DESTROY_BUFFER_CONSTANT{1.0};

    if (leftestPointOfMountain < currentLeftEdgeScreen - MountainClass::CHUNK_WIDTH - CHUNK_DESTROY_BUFFER_CONSTANT) {

        if (this->world.getHiker().getIsAlive()) {
            this->world.getMountain().generateNewChunk();
            std::cout << "Mountain chunk generated." << std::endl;
            // TODO graphics has to be adapted
            // graphics::generateChunkMesh(it.world());
        }
    }
}
void MountainGenerator::loadChunksOfMountain() {
    MountainClass &mountain = MountainClass::getInstance();

    for (std::size_t i{0}; i < MountainClass::NUMBER_OF_VERTICES / MountainClass::NUM_SECTIONS_PER_CHUNK; i++) {
        mountain.generateNewChunk();
        // TODO graphics has to be adapted
        // graphics::generateChunkMesh(world);
    }
    std::cout << "Loaded chunks of mountain" << std::endl;
}
