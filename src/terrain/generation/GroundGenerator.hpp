//
// Created by felix-roehr on 9/4/24.
//

#ifndef SURVIVING_SARNTAL_GROUNDGENERATOR_HPP
#define SURVIVING_SARNTAL_GROUNDGENERATOR_HPP

#include "../../game/DifficultyService.hpp"
#include "../biomes/BiomeType.hpp"
#include "../biomes/TerrainPhase.hpp"
#include "../components/Ground.hpp"
#include "ConstraintChecker.hpp"
#include "GenerationRenderer.hpp"

class GroundGenerator {
  public:
    GroundGenerator(const std::vector<TerrainPhase> &phaseChoices,
                    const TerrainPhase &firstPhase,
                    Vector basePoint,
                    Vector baseDerivative,
                    floatType startT,
                    floatType length,
                    HikerConstants &hikerConstants,
                    TerrainConstants &terrainConstants,
                    ResourceManager &resourceManager,
                    bool render);
    ~GroundGenerator() = default;

    /**
     * Generates a ground suitable for the given biome.
     */
    std::shared_ptr<Ground> generateGround();

  private:
    static constexpr int BASE_RETRACE_COUNT = 3;
    static constexpr floatType RETRACE_MULTIPLIER = 1.5f;
    static constexpr int RETRACE_ADDITION = 2;
    static constexpr int MAX_TRIES = 30;
    static constexpr int LEVEL_JUMP = 30;

    const TerrainConstants &terrainConstants;
    const HikerConstants &hikerConstants;

    int spawnedPhases = 0;

    const bool render;
    std::unique_ptr<GenerationRenderer> generationRenderer;

    const Vector basePoint;
    const Vector baseDerivative;
    const floatType startT;
    const floatType length;

    int retraceCount = 0;
    int retracedSteps = 0;
    bool retracing = false;

    int phasePointCount = 0;

    floatType minXPos = 0;

    std::shared_ptr<Ground> ground = nullptr;
    const std::vector<TerrainPhase> &phaseChoices;
    const TerrainPhase &firstPhase;

    TerrainPhase terrainPhase;
    Vector lastPoint;

    floatType renderingMinAngle = 0;
    floatType renderingMaxAngle = 0;
    std::shared_ptr<StaticPolyline> renderingGroundPolyRepresentation = nullptr;

    /**
     * Picks a new random terrain phase if the point count for the current terrainPhase is reached.
     */
    void updateTerrainPhase();

    /**
     * Generates the first two points of the ground
     */
    void generateGroundBase();

    /**
     * Deletes the last points generated from the terrain.
     */
    void retrace();

    /**
     * Checks whether the generation is currently retracing old steps and updates the parameter retracing accordingly.
     */
    void updateRetracing();

    /**
     * Generates a new point using the current last point as a starting point and the current terrain phase. If no
     * generation is possible that fulfills tha constraints with the already generated ground, no new point is returned.
     *
     * @return new point if possible, std::nullopt else
     */
    std::optional<Vector> generateNewPoint();

    /**
     * Generates a new point using the range provided by minAngle and maxAngle as well as the current terrain phase and
     * current last point. The validity of the new point is checked using the provided constraint checker.
     *
     * Here, we rotate the average direction by a random angle in the range [minAngle, maxAngle] and add the result to
     * the given basePoint.
     *
     * @param minAngle
     * @param maxAngle
     * @param constraintChecker
     * @return new point if possible, std::nullopt else
     */
    std::optional<Vector> generateNewPoint(floatType minAngle,
                                           floatType maxAngle,
                                           const std::shared_ptr<ConstraintChecker> &constraintChecker);

    /**
     * Returns a random phase from the phases of the biome corresponding to this ground generator
     *
     * @return the random phase
     */
    TerrainPhase getRandomPhase();
    int getCurrentDifficultyLevel() const;
    void reset();
    std::vector<TerrainPhase> getPossibleTerrainPhases();
};

#endif // SURVIVING_SARNTAL_GROUNDGENERATOR_HPP
