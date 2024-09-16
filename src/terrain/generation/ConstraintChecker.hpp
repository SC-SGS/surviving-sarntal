//
// Created by felix-roehr on 9/5/24.
//

#ifndef SURVIVING_SARNTAL_CONSTRAINTCHECKER_HPP
#define SURVIVING_SARNTAL_CONSTRAINTCHECKER_HPP

#include "../../utilities/vector.h"
#include "../components/Ground.hpp"

class ConstraintChecker {
  public:
    ConstraintChecker(const std::shared_ptr<Ground> &ground,
                      const std::shared_ptr<StaticPolyline> &groundPolyRepresentation,
                      const TerrainConstants &terrainConstants, const HikerConstants &hikerConstants);
    ~ConstraintChecker() = default;

    /**
     * Checks whether the given new point is a viable continuation of the current ground.
     *
     * @param newPoint
     * @return true, if all constraints are fulfilled
     */
    bool fulfillsConstraints(Vector newPoint, Vector newDerivative) const;

    /**
     * Checks whether the given new point fulfills the constraints for overhang height and depth.
     *
     * @param newPoint
     * @return true, if all overhang constraints are fulfilled
     */
    bool fulfillsOverhangConstraints(Vector newPoint) const;

  private:
    const HikerConstants hikerConstants;
    const TerrainConstants terrainConstants;

    const std::shared_ptr<Ground> ground;
    const std::shared_ptr<StaticPolyline> groundPolyRepresentation;
    const std::shared_ptr<StaticPolyline> groundBasePoints;
    floatType minXPos;

    bool fulfillsHikerClearance(const std::shared_ptr<StaticPolyline> &additionalTerrain) const;

    bool fulfillsNoSelfPenetration(const std::shared_ptr<StaticPolyline> &additionalTerrain) const;

    bool fulfillsMinimalBasePointDistance(const std::shared_ptr<StaticPolyline> &additionalTerrain,
                                          Vector newPoint) const;

    bool fulfillsMinimalBasePolylineAngle(Vector newPoint) const;

    bool fulfillsMaximumOverhangDepth(Vector newPoint) const;

    bool fulfillsOverhangJumpable(Vector newPoint) const;

    bool exceedsMinimalXPos(Vector newPoint) const;

    /**
     * Checks whether the new point is outside the range of the hiker clearance in the given direction from the old
     * point.
     *
     * @param newPoint
     * @param oldPoint
     * @param direction
     * @return
     */
    bool isOutsideHikerClearance(Vector oldPoint, Vector newPoint, Vector direction) const;

    /**
     * Checks whether the all points in the new terrain are outside the range of the hiker clearance in the given
     * direction from the old point.
     *
     * @param newPoint
     * @param oldPoint
     * @param direction
     * @return
     */
    bool isOutsideHikerClearance(Vector oldPoint, const std::shared_ptr<StaticPolyline> &newTerrain,
                                 Vector direction) const;

    /**
     * Checks whether the new point is outside the range of the hiker clearance in any direction from the old point.
     *
     * @param newPoint
     * @param oldPoint
     * @return
     */
    bool isOutsideHikerClearanceAnyDirection(Vector oldPoint, Vector newPoint) const;
};

#endif // SURVIVING_SARNTAL_CONSTRAINTCHECKER_HPP
