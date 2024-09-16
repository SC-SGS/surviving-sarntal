//
// Created by Aleksis Vezenkov on 29.06.24.
//

#ifndef SURVIVING_SARNTAL_YAMLCONVERSIONS_HPP
#define SURVIVING_SARNTAL_YAMLCONVERSIONS_HPP

#include "../game/GameProperties.hpp"
#include "raylib.h"
#include <yaml-cpp/yaml.h>

namespace YAML {
template <> struct convert<HikerConstants> {
    static bool decode(const Node &node, HikerConstants &hikerConstants) {
        if (!node.IsMap()) {
            return false;
        }

        hikerConstants.hikerMaxHealth = node["hikerMaxHealth"].as<int>();
        hikerConstants.hikerHeight = node["hikerHeight"].as<floatType>();
        hikerConstants.hikerWidth = node["hikerWidth"].as<floatType>();
        hikerConstants.crouchedHikerHeight = node["crouchedHikerHeight"].as<floatType>();
        hikerConstants.crouchedHikerWidth = node["crouchedHikerWidth"].as<floatType>();
        hikerConstants.mass = node["mass"].as<floatType>();
        hikerConstants.duckSpeedFactor = node["duckSpeedFactor"].as<floatType>();
        hikerConstants.normalSpeed = node["normalSpeed"].as<floatType>();
        hikerConstants.jumpVelocity = node["jumpVelocity"].as<floatType>();
        hikerConstants.airMovementSpeedFactor = node["airMovementSpeedFactor"].as<floatType>();
        hikerConstants.knockBack = node["knockBack"].as<floatType>();
        hikerConstants.minSpeedNegSlope = node["minSpeedNegSlope"].as<floatType>();
        hikerConstants.maxSpeedNegSlope = node["maxSpeedNegSlope"].as<floatType>();
        hikerConstants.minSpeedPosSlope = node["minSpeedPosSlope"].as<floatType>();
        hikerConstants.slowestNegSlope = node["slowestNegSlope"].as<floatType>();
        hikerConstants.fastestNegSlope = node["fastestNegSlope"].as<floatType>();
        hikerConstants.slowestPosScope = node["slowestPosScope"].as<floatType>();
        hikerConstants.maxClimbableSlope = node["maxClimbableSlope"].as<floatType>();

        return true;
    }
};

template <> struct convert<ItemsConstants> {
    static bool decode(const Node &node, ItemsConstants &itemsConstants) {
        if (!node.IsMap()) {
            return false;
        }

        itemsConstants.collectionRadius = node["collectionRadius"].as<floatType>();
        itemsConstants.slotsPerInventory = node["slotsPerInventory"].as<int>();
        itemsConstants.itemsPerSlot = node["itemsPerSlot"].as<int>();
        itemsConstants.kaiserschmarrnHealthRestoration = node["kaiserschmarrnHealthRestoration"].as<int>();
        itemsConstants.coinScore = node["coinScore"].as<int>();
        itemsConstants.startSpawnTime = node["startSpawnTime"].as<floatType>();
        itemsConstants.minSpawnTime = node["minSpawnTime"].as<int>();
        itemsConstants.maxSpawnTime = node["maxSpawnTime"].as<int>();
        itemsConstants.itemBaseHeight = node["itemBaseHeight"].as<floatType>();
        itemsConstants.itemMaxHeight = node["itemMaxHeight"].as<floatType>();
        itemsConstants.itemsPerSecond = node["itemsPerSecond"].as<floatType>();
        itemsConstants.inventorySlotSize = node["inventorySlotSize"].as<floatType>();

        return true;
    }
};

template <> struct convert<RockConstants> {
    static bool decode(const Node &node, RockConstants &rockConstants) {
        if (!node.IsMap()) {
            return false;
        }

        rockConstants.minRockSize = node["minRockSize"].as<floatType>();
        rockConstants.maxRockSize = node["maxRockSize"].as<floatType>();
        rockConstants.velocityCap = node["velocityCap"].as<floatType>();
        rockConstants.gamma = node["gamma"].as<floatType>();
        rockConstants.maxAngularVelocity = node["maxAngularVelocity"].as<floatType>();
        rockConstants.minSpawnVelocity = node["minSpawnVelocity"].as<floatType>();
        rockConstants.maxSpawnVelocity = node["maxSpawnVelocity"].as<floatType>();
        rockConstants.minSpawnRotationVelocity = node["minSpawnRotationVelocity"].as<floatType>();
        rockConstants.maxSpawnRotationVelocity = node["maxSpawnRotationVelocity"].as<floatType>();
        rockConstants.rockTimePeriodDifficult = node["rockTimePeriodDifficult"].as<floatType>();
        rockConstants.rockTimePeriodMedium = node["rockTimePeriodMedium"].as<floatType>();
        rockConstants.rockTimePeriodEasy = node["rockTimePeriodEasy"].as<floatType>();
        rockConstants.timeBetweenRockSpawns = node["timeBetweenRockSpawns"].as<floatType>();
        rockConstants.numOfRocksToSpawn = node["numOfRocksToSpawn"].as<int>();
        rockConstants.spawnOffsetX = node["spawnOffsetX"].as<floatType>();

        return true;
    }
};

template <> struct convert<PhysicsConstants> {
    static bool decode(const Node &node, PhysicsConstants &physicsConstants) {
        if (!node.IsMap()) {
            return false;
        }

        physicsConstants.gravitationalConstant = node["gravitationalConstant"].as<floatType>();
        physicsConstants.epsilon = node["epsilon"].as<floatType>();
        physicsConstants.rockTerrainDamping = node["rockTerrainDamping"].as<floatType>();
        physicsConstants.physicsDeltaT = node["physicsDeltaT"].as<floatType>();

        return true;
    }
};

template <> struct convert<InputConstants> {
    static bool decode(const Node &node, InputConstants &inputConstants) {
        if (!node.IsMap()) {
            return false;
        }

        inputConstants.maxGamepads = node["maxGamepads"].as<int>();

        return true;
    }
};

template <> struct convert<VisualConstants> {
    static bool decode(const Node &node, VisualConstants &visualConstants) {
        if (!node.IsMap()) {
            return false;
        }

        visualConstants.uiMargin = node["uiMargin"].as<int>();
        visualConstants.fontSizeScore = node["fontSizeScore"].as<int>();
        visualConstants.fontSizeAltimeter = node["fontSizeAltimeter"].as<int>();
        visualConstants.healthBarWidth = node["healthBarWidth"].as<int>();
        visualConstants.healthBarHeight = node["healthBarHeight"].as<int>();
        visualConstants.positionToScoreRatio = node["positionToScoreRatio"].as<int>();
        visualConstants.altimeterSteps = node["altimeterSteps"].as<int>();
        visualConstants.rumbleIntensity = node["rumbleIntensity"].as<floatType>();
        visualConstants.rumbleDampening = node["rumbleDampening"].as<floatType>();
        visualConstants.textureMidScale = node["textureMidScale"].as<floatType>();
        visualConstants.textureForeScale = node["textureForeScale"].as<floatType>();
        visualConstants.cameraToHikerOffset = node["cameraToHikerOffset"].as<int>();
        visualConstants.mountainResolution = node["mountainResolution"].as<int>();
        visualConstants.mountainGradientHeight = node["mountainGradientHeight"].as<int>();

        return true;
    }
};

template <> struct convert<BarriersConstants> {
    static bool decode(const Node &node, BarriersConstants &barriersConstants) {
        if (!node.IsMap()) {
            return false;
        }

        barriersConstants.killBarVelocity = node["killBarVelocity"].as<floatType>();
        barriersConstants.playerRightBarrierOffset = node["playerRightBarrierOffset"].as<floatType>();

        return true;
    }
};

template <> struct convert<MountainConstants> {
    static bool decode(const Node &node, MountainConstants &mountainConstants) {
        if (!node.IsMap()) {
            return false;
        }

        mountainConstants.visibleChunksCount = node["visibleChunkCount"].as<int>();
        mountainConstants.chunkBufferLeft = node["chunkBufferLeft"].as<int>();
        mountainConstants.chunkBufferRight = node["chunkBufferRight"].as<int>();
        mountainConstants.initialHeight = node["initialHeight"].as<floatType>();
        mountainConstants.slope = node["slope"].as<floatType>();
        mountainConstants.randomness = node["randomness"].as<floatType>();

        return true;
    }
};

template <> struct convert<TerrainConstants> {
    static bool decode(const Node &node, TerrainConstants &terrainConstants) {
        if (!node.IsMap()) {
            return false;
        }

        terrainConstants.renderingResolution = node["renderingResolution"].as<floatType>();
        terrainConstants.collisionDetectionResolution = node["collisionDetectionResolution"].as<floatType>();
        terrainConstants.biomeWidth = node["biomeWidth"].as<floatType>();
        terrainConstants.collisionDetectionPreFilteringTolerance =
            node["collisionDetectionPreFilteringTolerance"].as<floatType>();

        terrainConstants.generationCheckingRange = node["generationCheckingRange"].as<floatType>();
        terrainConstants.minimalBasePointDistance = node["minimalBasePointDistance"].as<floatType>();
        terrainConstants.minimalBasePolylineAngle = PI * (node["minimalBasePolylineAngle"].as<floatType>() / 180.0f);
        terrainConstants.maximalSlopeHikerClearance =
            PI * (node["maximalSlopeHikerClearance"].as<floatType>() / 180.0f);
        terrainConstants.hikerClearanceTolerance = node["hikerClearanceTolerance"].as<floatType>();
        terrainConstants.maximalOverhangDepth = node["maximalOverhangDepth"].as<floatType>();
        terrainConstants.maximalOverhangHeight = node["maximalOverhangHeight"].as<floatType>();

        terrainConstants.bufferLeft = node["bufferLeft"].as<floatType>();
        terrainConstants.bufferRight = node["bufferRight"].as<floatType>();

        return true;
    }
};

template <> struct convert<GameConstants> {
    static bool decode(const Node &node, GameConstants &gameConstants) {
        if (!node.IsMap()) {
            return false;
        }

        gameConstants.hikerConstants = node["hikerConstants"].as<HikerConstants>();
        gameConstants.itemsConstants = node["itemsConstants"].as<ItemsConstants>();
        gameConstants.rockConstants = node["rockConstants"].as<RockConstants>();
        gameConstants.physicsConstants = node["physicsConstants"].as<PhysicsConstants>();
        gameConstants.inputConstants = node["inputConstants"].as<InputConstants>();
        gameConstants.visualConstants = node["visualConstants"].as<VisualConstants>();
        gameConstants.barriersConstants = node["barriersConstants"].as<BarriersConstants>();
        gameConstants.terrainConstants = node["terrainConstants"].as<TerrainConstants>();

        return true;
    }
};
} // namespace YAML

#endif // SURVIVING_SARNTAL_YAMLCONVERSIONS_HPP