//
// Created by Anietta Weckauff on 13.05.24.
//
#include "MountainGenerator.h"

void MountainGenerator::generateMountainChunk(World *world) {
    float currentLeftEdgeScreen{world->getMonster().getXPosition()};
    float leftestPointOfMountain{
        world->getMountain().getVertex(world->getMountain().getIndexIntervalOfEntireMountain().startIndex).x};
    constexpr float CHUNK_DESTROY_BUFFER_CONSTANT{1.0};

    if (leftestPointOfMountain < currentLeftEdgeScreen - MountainClass::CHUNK_WIDTH - CHUNK_DESTROY_BUFFER_CONSTANT) {

        if (world->getHiker().getIsAlive()) {
            world->getMountain().generateNewChunk();
            // TODO graphics has to be adapted
            // graphics::generateChunkMesh(it.world());
        }
    }
}
void MountainGenerator::loadChunksOfMountain(World *world) {
    MountainClass mountain = world->getMountain();

    for (std::size_t i{0}; i < MountainClass::NUMBER_OF_VERTICES / MountainClass::NUM_SECTIONS_PER_CHUNK; i++) {
        mountain.generateNewChunk();
        // TODO graphics has to be adapted
        // graphics::generateChunkMesh(world);
    }
}
