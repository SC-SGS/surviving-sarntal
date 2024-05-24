//
// Created by Anietta Weckauff on 13.05.24.
//

#ifndef SURVIVING_SARNTAL_MOUNTAINGENERATOR_H
#define SURVIVING_SARNTAL_MOUNTAINGENERATOR_H

#include "../entities/World.h"

/**
 * This class is responsible for generating the mountain chunk by chunk.
 */

class MountainGenerator {

  private:
  public:
    /**
     * Checks if a new chunk of the mountain needs to be generated and lets the
     * mountain generate a new chunk if needed.
     * @param world
     */
    void generateMountainChunk(World *world);

    /**
     *
     * @param world
     */
    void loadChunksOfMountain(World *world);
};

#endif // SURVIVING_SARNTAL_MOUNTAINGENERATOR_H
