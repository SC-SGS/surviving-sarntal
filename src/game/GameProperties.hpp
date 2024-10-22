//
// Created by Aleksis Vezenkov on 20.06.24.
//

#ifndef SURVIVING_SARNTAL_GAMEPROPERTIES_HPP
#define SURVIVING_SARNTAL_GAMEPROPERTIES_HPP

#include "raylib.h"
#include <cstddef>
#include <vector>

typedef float floatType;
constexpr floatType NUMERIC_EPSILON = 0.0001f;

namespace graphics {
constexpr int SCREEN_WIDTH_IN_PIXEL = 1600;
constexpr int SCREEN_HEIGHT_IN_PIXEL = 900;
constexpr float UNIT_TO_PIXEL_RATIO = 77.5;
}; // namespace graphics

struct HikerConstants {
    int hikerMaxHealth;

    floatType spawnXRelativeToScreenWidth;

    // Hiker size
    floatType hikerHeight;
    floatType hikerWidth;
    floatType crouchedHikerHeight;
    floatType crouchedHikerWidth;

    floatType mass;

    // Hiker movement speed
    floatType duckSpeedFactor;
    floatType normalSpeed;
    floatType maxSpeed;
    floatType jumpVelocity;
    floatType airMovementSpeedFactor;
    floatType knockbackIntensity;
    floatType knockbackLossPerStep;
    floatType knockbackCutoff;
    floatType maxSpeedFactorNegSlope;

    // Terrain interaction
    floatType maxClimbableSlope;
};

struct ItemsConstants {
    bool spawnItems;
    floatType shieldTime;
    floatType collectionRadius;
    int slotsPerInventory;
    int itemsPerSlot;
    int kaiserschmarrnHealthRestoration;
    int coinScore;
    floatType coinAccelerationFactor;

    floatType startSpawnTime;
    int minSpawnTime;
    int maxSpawnTime;

    floatType itemBaseSpawnHeight;
    floatType itemMaxSpawnHeight;
    floatType inventorySlotSize;
};

struct RockConstants {
    bool spawnRocks;

    // Rock size
    floatType minRockSize;
    floatType maxRockSize;

    // Rock speed
    floatType minSpawnLinearMomentum;
    floatType maxSpawnLinearMomentum;
    floatType minSpawnAngularMomentum;
    floatType maxSpawnAngularMomentum;

    // Rock spawn
    floatType minRockDensity;
    floatType maxRockDensity;

    floatType spawnOffsetX;
};

struct PhysicsConstants {
    floatType gravitationalConstant;
    floatType gravitySpinEffectOnVertices;
    floatType epsilon;
    floatType rockRockBounciness;
    floatType rockTerrainBounciness;
    floatType physicsDeltaT;
    floatType terrainSubstepSize;
    floatType rockSubstepSize;
    size_t maxNumberOfResolutionSteps;
    size_t maxNumberOfPhysicsStepsPerFrame;
    bool debugCDRendering;
};

struct InputConstants {
    int maxGamepads;
    floatType gamepadItemSwitchCooldown;
    int gamepadInitializingTime;
    int gamepadInitializingCountdownFontSize;
    floatType itemSwitchThreshold;
};

struct VisualConstants {
    int uiMargin;
    int fontSizeScore;
    int fontSizeAltimeter;
    int healthBarWidth;
    int healthBarHeight;

    int positionToScoreRatio;

    int altimeterSteps;

    floatType rumbleIntensity;
    floatType rumbleDampening;

    float backgroundDistanceScale;
    int cameraToHikerOffset;
    int mountainGradientHeight;
    int mountainResolution;

    bool shakeEnabled;

    bool renderPhysicsStepSize;
    int worldSize;
};

struct AudioConstants {
    bool musicEnabled;
    float musicVolume;

    floatType effectsThreshold;
};

struct BarriersConstants {
    floatType killBarBaseVelocity;
    floatType killBarAccelerationFactor;
    floatType killBarMaxVelocity;
    floatType monsterXRelativeToScreenWidth;
    floatType monsterWidth;
    floatType monsterHeight;
};

struct TerrainConstants {
    floatType bufferLeft;
    floatType bufferRight;
    floatType biomeWidth;

    floatType renderingResolution;
    floatType collisionDetectionResolution;
    floatType collisionDetectionPreFilteringTolerance;
    floatType generationCheckingRange;

    floatType minimalBasePointDistance;
    floatType minimalBasePolylineAngle;
    floatType maximalSlopeHikerClearance;
    floatType hikerClearanceTolerance;
    floatType maximalOverhangDepth;
    floatType maximalOverhangHeight;
};

struct RockSpawnerConstants {

    int maxNumberOfRocksToSpawn;

    /**
     * A list that contains the factor that determines the difficulty
     */
    std::vector<floatType> linearMomentumDifficultyFactor;

    /**
     * A list that contains the x position at which the spawning phase should start at.
     * The index of the value represents the difficulty level. For example 1 -> EASY difficulty.
     *
     */
    std::vector<floatType> spawningPhase;

    /**
     *
     */
    std::vector<floatType> rockSpawnTimeInterval;

    /**
     *
     */
    std::vector<floatType> rockTypePhase;
    floatType minAngularMomentum;
    floatType maxAngularMomentum;
    int minNumPointsForGeneration;
    int maxNumPointsForGeneration;
    floatType minRandYSpawnOffset;
    floatType maxRandYSpawnOffset;
};

struct DifficultyConstants {
    int highestDifficultyLevel;
    int difficultyFactor;
    int difficultyStartingPoint;
};

struct GameConstants {
    HikerConstants hikerConstants{};
    ItemsConstants itemsConstants{};
    RockConstants rockConstants{};
    PhysicsConstants physicsConstants{};
    InputConstants inputConstants{};
    VisualConstants visualConstants{};
    BarriersConstants barriersConstants{};
    TerrainConstants terrainConstants{};
    RockSpawnerConstants rockSpawnerConstants{};
    AudioConstants audioConstants{};
    DifficultyConstants difficultyConstants{};
};

#endif // SURVIVING_SARNTAL_GAMEPROPERTIES_HPP