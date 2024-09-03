//
// Created by Aleksis Vezenkov on 20.06.24.
//

#ifndef SURVIVING_SARNTAL_GAMEPROPERTIES_HPP
#define SURVIVING_SARNTAL_GAMEPROPERTIES_HPP

typedef float floatType;

namespace graphics {
constexpr int SCREEN_WIDTH = 1600;
constexpr int SCREEN_HEIGHT = 900;
}; // namespace graphics

struct HikerConstants {
    int hikerMaxHealth;

    // Hiker size
    floatType hikerHeight;
    floatType hikerWidth;
    floatType crouchedHikerHeight;
    floatType crouchedHikerWidth;

    // Hiker movement speed
    floatType duckSpeedFactor;
    floatType normalSpeed;
    floatType jumpVelocity;
    floatType airMovementSpeedFactor;
    floatType knockBack;

    floatType minSpeedNegSlope;
    floatType maxSpeedNegSlope;
    floatType minSpeedPosSlope;

    floatType slowestNegSlope;
    floatType fastestNegSlope;
    floatType slowestPosScope;

    floatType maxClimbableSlope;
};

struct ItemsConstants {
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
    floatType generationCheckingRange;

    floatType minimalBasePointDistance;
    floatType minimalBasePolylineAngle;
    floatType maximalSlopeHikerClearance;
    floatType hikerClearanceTolerance;
};

struct MountainConstants {
    int visibleChunksCount = 16;
    int chunkBufferLeft = 2;
    int chunkBufferRight = 16;
    int chunkCount = visibleChunksCount + chunkBufferLeft + chunkBufferRight;

    floatType chunkWidth = static_cast<floatType>(1.0 * graphics::SCREEN_WIDTH / visibleChunksCount);
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
};

#endif // SURVIVING_SARNTAL_GAMEPROPERTIES_HPP