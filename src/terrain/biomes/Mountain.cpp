//
// Created by felix-roehr on 7/29/24.
//

#include "Mountain.hpp"

const std::vector<TerrainPhase> Mountain::mountainPhaseChoices = {
    // smooth and even
    {{4.0f, 0.0f}, 0.3f, 10},

    // slightly uphill
    {{3.0f, 0.6f}, 0.6f, 10},

    // steep uphill
    {{2.0f, 1.4f}, 0.5f, 8},

    // steep downhill
    {{2.0f, -1.4f}, 0.5f, 8},

    // slightly downhill
    {{3.0f, -0.6f}, 0.6f, 10},

    // overhang
    {{-2.5f, 0.0f}, 0.5f, 5}};

const TerrainPhase Mountain::mountainFirstPhase = {{3.0f, 0.5f}, 0.0f, 3};

Mountain::Mountain(Vector basePoint, Vector baseDerivative, floatType startT, floatType length,
                   HikerConstants hikerConstants, TerrainConstants terrainConstants, ResourceManager &resourceManager,
                   bool renderGeneration)
    : Biome(Mountain::mountainPhaseChoices, Mountain::mountainFirstPhase, basePoint, baseDerivative, startT, length,
            hikerConstants, terrainConstants, resourceManager, renderGeneration) {}