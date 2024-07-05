//
// Created by Anietta Weckauff on 13.05.24.
//

#ifndef SURVIVING_SARNTAL_MOUNTAINGENERATOR_H
#define SURVIVING_SARNTAL_MOUNTAINGENERATOR_H

#include "../entities/World.h"
#include "../utilities/Singleton.hpp"

/**
 * This class is responsible for generating the mountain chunk by chunk.
 */

class MountainGenerator { // Allow Singleton to access the constructor??

  public:
    explicit MountainGenerator(World &world);
    ~MountainGenerator() = default;
    /**
     * Checks if a new chunk of the mountain needs to be generated and lets the
     * mountain generate a new chunk if needed.
     */
    void generateMountainChunk();

    /**
     * This method is responsible for loading the initial chunks of the mountain
     * when the game is first started.
     */
    void loadChunksOfMountain();
    floatType getTerrainHeight(floatType xVal, floatType yVal, floatType ridge_height, floatType baseline = 0.0f);
    void generateChunkMesh();
    static Vector3 computeNormal(Vector3 pos1, Vector3 pos2, Vector3 pos3);

  private:
    World &world;
};

#endif // SURVIVING_SARNTAL_MOUNTAINGENERATOR_H
