//
// Created by felix-roehr on 7/29/24.
//

#include "Mountain.hpp"

const std::vector<TerrainPhase> Mountain::mountainPhaseChoices = {
    // slightly uphill - OK
    {{3.0f, 1.5f}, 0.2f, 10, 0, 25},

    // slightly downhill - OK TODO Do we want this?
    {{3.0f, -0.5f}, 0.4f, 8, 4},

    // steep uphill - OK
    {{3.0f, 3.5f}, 0.3f, 8, 1, 10},

    // steep downhill - OK
    {{3.0f, -2.5f}, 0.4f, 5, 2},

    // overhang
    {{-2.5f, 0.0f}, 0.5f, 5, 0, 5}};

const TerrainPhase Mountain::mountainFirstPhase = {{3.0f, 0.5f}, 0.0f, 3, 0, 0};

Mountain::Mountain(Vector basePoint,
                   Vector baseDerivative,
                   floatType startT,
                   floatType length,
                   HikerConstants hikerConstants,
                   TerrainConstants terrainConstants,
                   ResourceManager &resourceManager,
                   bool renderGeneration)
    : Biome(Mountain::mountainPhaseChoices,
            Mountain::mountainFirstPhase,
            basePoint,
            baseDerivative,
            startT,
            length,
            hikerConstants,
            terrainConstants,
            resourceManager,
            renderGeneration) {}