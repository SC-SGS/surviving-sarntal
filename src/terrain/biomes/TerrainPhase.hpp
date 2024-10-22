//
// Created by felix-roehr on 8/5/24.
//

#ifndef SURVIVING_SARNTAL_TERRAINPHASE_HPP
#define SURVIVING_SARNTAL_TERRAINPHASE_HPP

#include "../../utilities/vector.h"

struct TerrainPhase {
    Vector avgDirection;
    floatType randomness;
    int pointCount;
    int difficultyLevel;
    int spawnWeight;
};

#endif // SURVIVING_SARNTAL_TERRAINPHASE_HPP
