//
// Created by Aleksis Vezenkov on 29.06.24.
//

#ifndef SURVIVING_SARNTAL_YAMLCONVERSIONS_HPP
#define SURVIVING_SARNTAL_YAMLCONVERSIONS_HPP

#include "../entities/Rock.h"
#include "../game/GameProperties.hpp"
#include "../game/Simulation.h"
#include "raylib.h"
#include <yaml-cpp/yaml.h>

namespace YAML {
template <> struct convert<HikerConstants> {
    static bool decode(const Node &node, HikerConstants &hikerConstants) {
        if (!node.IsMap()) {
            return false;
        }

        hikerConstants.hikerMaxHealth = node["hikerMaxHealth"].as<int>();
        hikerConstants.spawnXRelativeToScreenWidth = node["spawnXRelativeToScreenWidth"].as<floatType>();
        hikerConstants.hikerHeight = node["hikerHeight"].as<floatType>();
        hikerConstants.hikerWidth = node["hikerWidth"].as<floatType>();
        hikerConstants.crouchedHikerHeight = node["crouchedHikerHeight"].as<floatType>();
        hikerConstants.crouchedHikerWidth = node["crouchedHikerWidth"].as<floatType>();
        hikerConstants.mass = node["mass"].as<floatType>();
        hikerConstants.duckSpeedFactor = node["duckSpeedFactor"].as<floatType>();
        hikerConstants.normalSpeed = node["normalSpeed"].as<floatType>();
        hikerConstants.maxSpeed = node["maxSpeed"].as<floatType>();
        hikerConstants.jumpVelocity = node["jumpVelocity"].as<floatType>();
        hikerConstants.airMovementSpeedFactor = node["airMovementSpeedFactor"].as<floatType>();
        hikerConstants.knockbackIntensity = node["knockbackIntensity"].as<floatType>();
        hikerConstants.maxSpeedNegSlope = node["maxSpeedNegSlope"].as<floatType>();
        hikerConstants.maxClimbableSlope = node["maxClimbableSlope"].as<floatType>();
        hikerConstants.knockbackLossPerStep = node["knockbackLossPerStep"].as<floatType>();
        hikerConstants.knockbackCutoff = node["knockbackCutoff"].as<floatType>();

        return true;
    }
};

template <> struct convert<ItemsConstants> {
    static bool decode(const Node &node, ItemsConstants &itemsConstants) {
        if (!node.IsMap()) {
            return false;
        }

        itemsConstants.spawnItems = node["spawnItems"].as<bool>();
        itemsConstants.shieldTime = node["shieldTime"].as<floatType>();
        itemsConstants.collectionRadius = node["collectionRadius"].as<floatType>();
        itemsConstants.slotsPerInventory = node["slotsPerInventory"].as<int>();
        itemsConstants.itemsPerSlot = node["itemsPerSlot"].as<int>();
        itemsConstants.kaiserschmarrnHealthRestoration = node["kaiserschmarrnHealthRestoration"].as<int>();
        itemsConstants.coinScore = node["coinScore"].as<int>();
        itemsConstants.coinAccelerationFactor = node["coinAccelerationFactor"].as<floatType>();
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

        rockConstants.spawnRocks = node["spawnRocks"].as<bool>();
        rockConstants.minRockSize = node["minRockSize"].as<floatType>();
        rockConstants.maxRockSize = node["maxRockSize"].as<floatType>();
        rockConstants.minSpawnLinearMomentum = node["minSpawnLinearMomentum"].as<floatType>();
        rockConstants.maxSpawnLinearMomentum = node["maxSpawnLinearMomentum"].as<floatType>();
        rockConstants.minRockDensity = node["minRockDensity"].as<floatType>();
        rockConstants.maxRockDensity = node["maxRockDensity"].as<floatType>();
        rockConstants.minSpawnAngularMomentum = node["minSpawnAngularMomentum"].as<floatType>();
        rockConstants.maxSpawnAngularMomentum = node["maxSpawnAngularMomentum"].as<floatType>();
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
        physicsConstants.gravitySpinEffectOnVertices = node["gravitySpinEffectOnVertices"].as<floatType>();
        physicsConstants.epsilon = node["epsilon"].as<floatType>();
        physicsConstants.rockRockBounciness = node["rockRockBounciness"].as<floatType>();
        physicsConstants.rockTerrainBounciness = node["rockTerrainBounciness"].as<floatType>();
        physicsConstants.physicsDeltaT = node["physicsDeltaT"].as<floatType>();
        physicsConstants.terrainSubstepSize = node["terrainSubstepSize"].as<floatType>();
        physicsConstants.rockSubstepSize = node["rockSubstepSize"].as<floatType>();
        physicsConstants.maxNumberOfResolutionSteps = node["maxNumberOfResolutionSteps"].as<size_t>();
        physicsConstants.maxNumberOfResolutionSteps = node["maxNumberOfPhysicsStepsPerFrame"].as<size_t>();
        physicsConstants.debugCDRendering = node["debugCDRendering"].as<bool>();

        return true;
    }
};

template <> struct convert<InputConstants> {
    static bool decode(const Node &node, InputConstants &inputConstants) {
        if (!node.IsMap()) {
            return false;
        }

        inputConstants.maxGamepads = node["maxGamepads"].as<int>();
        inputConstants.gamepadItemSwitchCooldown = node["gamepadItemSwitchCooldown"].as<floatType>();
        inputConstants.gamepadInitializingTime = node["gamepadInitializingTime"].as<int>();
        inputConstants.gamepadInitializingCountdownFontSize = node["gamepadInitializingCountdownFontSize"].as<int>();
        inputConstants.itemSwitchThreshold = node["itemSwitchThreshold"].as<floatType>();

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
        visualConstants.backgroundDistanceScale = node["backgroundDistanceScale"].as<floatType>();
        visualConstants.cameraToHikerOffset = node["cameraToHikerOffset"].as<int>();
        visualConstants.mountainResolution = node["mountainResolution"].as<int>();
        visualConstants.mountainGradientHeight = node["mountainGradientHeight"].as<int>();
        visualConstants.shakeEnabled = node["shakeEnabled"].as<bool>();
        visualConstants.renderPhysicsStepSize = node["renderPhysicsStepSize"].as<bool>();

        return true;
    }
};

template <> struct convert<BarriersConstants> {
    static bool decode(const Node &node, BarriersConstants &barriersConstants) {
        if (!node.IsMap()) {
            return false;
        }

        barriersConstants.killBarBaseVelocity = node["killBarBaseVelocity"].as<floatType>();
        barriersConstants.killBarAccelerationFactor = node["killBarAccelerationFactor"].as<floatType>();
        barriersConstants.maxKillBarFactor = node["maxKillBarFactor"].as<floatType>();
        barriersConstants.monsterXRelativeToScreenWidth = node["monsterXRelativeToScreenWidth"].as<floatType>();
        barriersConstants.monsterWidth = node["monsterWidth"].as<floatType>();
        barriersConstants.monsterHeight = node["monsterHeight"].as<floatType>();

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

template <> struct convert<RockSpawnerConstants> {
    static bool decode(const Node &node, RockSpawnerConstants &rockSpawnerConstants) {
        if (!node.IsMap()) {
            return false;
        }

        rockSpawnerConstants.linearMomentumDifficultyFactor =
            node["linearMomentumDifficultyFactor"].as<std::vector<floatType>>();
        rockSpawnerConstants.spawningPhase = node["spawningPhase"].as<std::vector<floatType>>();
        rockSpawnerConstants.rockSpawnTimeInterval = node["rockSpawnTimeInterval"].as<std::vector<floatType>>();
        rockSpawnerConstants.rockTypePhase = node["rockTypePhase"].as<std::vector<floatType>>();

        return true;
    }
};

template <> struct convert<AudioConstants> {
    static bool decode(const Node &node, AudioConstants &audioConstants) {
        if (!node.IsMap()) {
            return false;
        }

        audioConstants.musicEnabled = node["musicEnabled"].as<bool>();
        audioConstants.musicVolume = node["musicVolume"].as<floatType>();
        audioConstants.effectsThreshold = node["effectsThreshold"].as<floatType>();

        return true;
    }
};

template <> struct convert<SimulationProperties> {
    static bool decode(const Node &node, SimulationProperties &simProperties) {
        if (!node.IsMap()) {
            return false;
        }

        simProperties.duration = node["duration"].as<double>();
        simProperties.speed = node["speed"].as<double>();
        simProperties.dropPosition = node["dropPosition"].as<Vector>();
        simProperties.maxNumberOfRocks = node["maxNumberOfRocks"].as<size_t>();

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
        gameConstants.rockSpawnerConstants = node["rockSpawnerConstants"].as<RockSpawnerConstants>();
        gameConstants.audioConstants = node["audioConstants"].as<AudioConstants>();

        return true;
    }
};

template <> struct convert<Vector> {
    static Node encode(const Vector &rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        return node;
    }

    static bool decode(const Node &node, Vector &rhs) {
        if (!node.IsSequence() || node.size() != 2) {
            return false;
        }

        rhs.x = node[0].as<floatType>();
        rhs.y = node[1].as<floatType>();
        return true;
    }
};

} // namespace YAML

#endif // SURVIVING_SARNTAL_YAMLCONVERSIONS_HPP