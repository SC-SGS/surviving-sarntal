//
// Created by Aleksis Vezenkov on 20.06.24.
//

#ifndef SURVIVING_SARNTAL_GAMEPROPERTIES_HPP
#define SURVIVING_SARNTAL_GAMEPROPERTIES_HPP

#include <iostream>
#include <vector>

typedef float floatType;

namespace graphics {
constexpr int SCREEN_WIDTH_IN_PIXEL = 1600;
constexpr int SCREEN_HEIGHT_IN_PIXEL = 900;
constexpr float UNIT_TO_PIXEL_RATIO = 77.5;
constexpr floatType SCREEN_WIDTH_IN_METER = SCREEN_WIDTH_IN_PIXEL / UNIT_TO_PIXEL_RATIO;
constexpr floatType SCREEN_HEIGHT_IN_METER = SCREEN_HEIGHT_IN_PIXEL / UNIT_TO_PIXEL_RATIO;
}; // namespace graphics

struct HikerConstants {
    int hikerMaxHealth;

    // Hiker size
    floatType hikerHeight;
    floatType hikerWidth;
    floatType crouchedHikerHeight;
    floatType crouchedHikerWidth;

    floatType mass;

    // Hiker movement speed
    floatType duckSpeedFactor;
    floatType normalSpeed;
    floatType jumpVelocity;
    floatType airMovementSpeedFactor;
    floatType knockBack;
    floatType maxSpeedNegSlope;

    // Terrain interaction
    floatType maxClimbableSlope;
    floatType terrainCollisionDampening; // Factor by which the reflected velocity of the hiker is dampened, when they
                                         // hit the terrain.
    floatType friction;
};

struct ItemsConstants {
    bool spawnItems;
    floatType shieldTime;
    floatType collectionRadius;
    int slotsPerInventory;
    int itemsPerSlot;
    int kaiserschmarrnHealthRestoration;
    int coinScore;

    floatType startSpawnTime;
    int minSpawnTime;
    int maxSpawnTime;

    floatType itemBaseHeight;
    floatType itemMaxHeight;
    floatType itemsPerSecond;
    floatType inventorySlotSize;
};

struct RockConstants {
    bool spawnRocks;

    // Rock size
    floatType minRockSize;
    floatType maxRockSize;

    // Rock speed
    floatType velocityCap;
    floatType gamma;
    floatType maxAngularVelocity;
    floatType minSpawnVelocity;
    floatType maxSpawnVelocity;
    floatType minSpawnRotationVelocity;
    floatType maxSpawnRotationVelocity;

    // Rock spawn
    floatType rockTimePeriodDifficult;
    floatType rockTimePeriodMedium;
    floatType rockTimePeriodEasy;
    floatType timeBetweenRockSpawns;
    int numOfRocksToSpawn;
    floatType minRockDensity;
    floatType maxRockDensity;

    floatType spawnOffsetX;
};

struct PhysicsConstants {
    floatType gravitationalConstant;
    floatType epsilon;
    floatType rockTerrainDamping;
    floatType physicsDeltaT;
};

struct InputConstants {
    int maxGamepads;
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

    floatType textureMidScale;
    floatType textureForeScale;
    int cameraToHikerOffset;
    int mountainGradientHeight;
    int mountainResolution;
};

struct BarriersConstants {
    floatType killBarVelocity;
    floatType playerRightBarrierOffset;
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

struct MountainConstants {
    int visibleChunksCount = 16;
    int chunkBufferLeft = 2;
    int chunkBufferRight = 16;
    int chunkCount = visibleChunksCount + chunkBufferLeft + chunkBufferRight;

    floatType chunkWidth =
        static_cast<floatType>(1.0 * graphics::SCREEN_WIDTH_IN_PIXEL /
                               (static_cast<float>(visibleChunksCount) * graphics::UNIT_TO_PIXEL_RATIO));
    floatType width = static_cast<floatType>(chunkCount) * chunkWidth;
    floatType start = static_cast<floatType>(-chunkBufferLeft) * chunkWidth;
    floatType initialHeight = 0.0;

    floatType slope = 0.4f;
    floatType randomness = 1.0;
    floatType newPointDeltaStandard = slope * chunkWidth;
    floatType newPointDeltaMin = newPointDeltaStandard - (randomness * chunkWidth);
    floatType newPointDeltaMax = newPointDeltaStandard + (randomness * chunkWidth);
    floatType linStepSize = 0.5f;
};

struct RockSpawnerConstants {

    /**
     * A list that contains the factor that determinse
     */
    std::vector<floatType> velocityDifficultyFactor;

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
};

struct GameConstants {
    HikerConstants hikerConstants{};
    ItemsConstants itemsConstants{};
    RockConstants rockConstants{};
    PhysicsConstants physicsConstants{};
    InputConstants inputConstants{};
    VisualConstants visualConstants{};
    BarriersConstants barriersConstants{};
    MountainConstants mountainConstants{};
    TerrainConstants terrainConstants{};
    RockSpawnerConstants rockSpawnerConstants{};
};

#endif // SURVIVING_SARNTAL_GAMEPROPERTIES_HPP