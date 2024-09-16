//
// Created by felix-roehr on 9/5/24.
//

#ifndef SURVIVING_SARNTAL_GENERATIONANGLECALCULATOR_HPP
#define SURVIVING_SARNTAL_GENERATIONANGLECALCULATOR_HPP

#include "../../game/GameProperties.hpp"
#include "../biomes/TerrainPhase.hpp"
#include "ConstraintChecker.hpp"
class GenerationAngleCalculator {
  public:
    GenerationAngleCalculator(const std::shared_ptr<Ground> &ground,
                              const std::shared_ptr<StaticPolyline> &groundPolyRepresentation, TerrainPhase &phase,
                              const HikerConstants &hikerConstants, const TerrainConstants &terrainConstants);
    ~GenerationAngleCalculator() = default;

    /**
     * Calculates the maximal angle such that a point generated using this angle fulfills all constraints.
     * The new point is generated using the lasOldPoint as the basis point and the angle is applied to the
     * average direction in the terrain phase.
     *
     * The method uses an iterative bisection process to find this maximum.
     *
     * @return the maximal angle if it exists, nullopt else
     */
    std::optional<floatType> calculateMinAngle();

    /**
     * Calculates the minimal angle such that a point generated using this angle fulfills all constraints.
     * The new point is generated using the lasOldPoint as the basis point and the angle is applied to the
     * average direction in the terrain phase.
     *
     * The method uses an iterative bisection process to find this minimum.
     *
     * @return the minimal angle if it exists, nullopt else
     */
    std::optional<floatType> calculateMaxAngle();

  private:
    // const HikerConstants hikerConstants;
    // const TerrainConstants terrainConstants;

    const floatType minAllowedAngle;
    const floatType maxAllowedAngle;
    const TerrainPhase &phase;
    const std::shared_ptr<Ground> ground;
    const Vector lastOldPoint;
    const std::shared_ptr<StaticPolyline> groundPolyRepresentation;

    static constexpr floatType MIN_STEP = 0.01f;
    static constexpr int MAX_TRIES = 30;

    const std::shared_ptr<ConstraintChecker> constraintChecker;

  public:
    std::shared_ptr<ConstraintChecker> getConstraintChecker() const;

  private:
    floatType deltaMaxAngle;
    floatType deltaMinAngle;
    floatType maxAngle;
    floatType minAngle;

    void updateMaxAngle(Vector newPoint);

    void updateMinAngle(Vector newPoint);
};

#endif // SURVIVING_SARNTAL_GENERATIONANGLECALCULATOR_HPP
