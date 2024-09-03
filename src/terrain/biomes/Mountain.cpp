//
// Created by felix-roehr on 7/29/24.
//

#include "Mountain.hpp"

const std::vector<TerrainPhase> Mountain::mountainPhaseChoices = {
    // smooth and even
    {{400.0f, 0.0f}, 0.3f, 10},

    // slightly uphill
    {{300.0f, 60.0f}, 0.6f, 10},

    // steep uphill
    {{200.0f, 140.0f}, 0.5f, 8},

    // steep downhill
    {{200.0f, -140.0f}, 0.5f, 8},

    // slightly downhill
    {{300.0f, -60.0f}, 0.6f, 10},

    // overhang
    {{-250.0f, 0.0f}, 0.5f, 5}};

Mountain::Mountain(Vector basePoint, Vector baseDerivative, floatType startT, floatType length,
                   HikerConstants hikerConstants, TerrainConstants terrainConstants)
    : Biome(Mountain::mountainPhaseChoices, basePoint, baseDerivative, startT, length, hikerConstants,
            terrainConstants) {}