//
// Created by felix-roehr on 9/5/24.
//

#include "ConstraintChecker.hpp"
#include <algorithm>
#include <cmath>

ConstraintChecker::ConstraintChecker(const std::shared_ptr<Ground> &ground,
                                     const std::shared_ptr<StaticPolyline> &groundPolyRepresentation,
                                     const TerrainConstants &terrainConstants,
                                     const HikerConstants &hikerConstants)
    : terrainConstants(terrainConstants),
      hikerConstants(hikerConstants),
      ground(ground),
      groundPolyRepresentation(groundPolyRepresentation),
      groundBasePoints(ground->getBasePoints()) {
    this->minXPos = this->groundBasePoints->getStartPoint().x;
    Vector lastPos = this->groundBasePoints->getStartPoint();
    bool overhang = false;
    for (auto const &basePoint : this->groundBasePoints->getPoints()) {
        Vector delta = basePoint - lastPos;
        if (basePoint.x > lastPos.x && delta.computeSlope() < this->hikerConstants.maxClimbableSlope) {
            if (overhang) { // first point after an overhang is new minimal x pos
                this->minXPos = basePoint.x + this->hikerConstants.hikerWidth;
            }
            overhang = false;
        } else {
            overhang = true;
        }
        lastPos = basePoint;
    }
}

bool ConstraintChecker::fulfillsConstraints(Vector newPoint, Vector newDerivative) const {
    std::shared_ptr<StaticPolyline> additionalPolyline = this->ground->getPolyRepresentationLastPointToNewPoint(
        newPoint, newDerivative, this->terrainConstants.collisionDetectionResolution);
    return this->fulfillsHikerClearance(additionalPolyline) &&
           this->fulfillsMinimalBasePointDistance(additionalPolyline, newPoint) &&
           this->fulfillsMinimalBasePolylineAngle(newPoint) && this->fulfillsNoSelfPenetration(additionalPolyline) &&
           this->fulfillsMaximumOverhangDepth(newPoint) && this->fulfillsOverhangJumpable(newPoint) &&
           this->exceedsMinimalXPos(newPoint);
}

bool ConstraintChecker::fulfillsHikerClearance(const std::shared_ptr<StaticPolyline> &additionalTerrain) const {
    Vector lastPointOldTerrain = this->groundPolyRepresentation->getPoints().back();
    for (int index = 0; index < this->groundPolyRepresentation->getPoints().size() - 1; index++) {
        Vector prevPoint = this->groundPolyRepresentation->getPoints().at(index);
        Vector currentPoint = this->groundPolyRepresentation->getPoints().at(index + 1);
        Vector direction = currentPoint - prevPoint;
        if (this->isOutsideHikerClearanceAnyDirection(currentPoint, lastPointOldTerrain)) {
            if (!this->isOutsideHikerClearance(prevPoint, additionalTerrain, direction)) {
                return false;
            }
        }
    }
    return true;
}

bool ConstraintChecker::fulfillsNoSelfPenetration(const std::shared_ptr<StaticPolyline> &additionalTerrain) const {
    return true;
    return !this->groundPolyRepresentation->intersectsWithoutFirstPoint(*additionalTerrain);
}

bool ConstraintChecker::fulfillsMaximumOverhangDepth(Vector newPoint) const {
    floatType depth = 0;
    for (int index = 0; index < this->groundPolyRepresentation->getPoints().size() - 1; index++) {
        Vector start = this->groundPolyRepresentation->getPoints().at(index);
        Vector end = this->groundPolyRepresentation->getPoints().at(index + 1);
        Vector delta = end - start;
        if (delta.x <= 0 || delta.computeSlope() >= this->hikerConstants.maxClimbableSlope) {
            depth += delta.x;
        } else {
            depth = 0;
        }
        if (fabs(depth) > this->terrainConstants.maximalOverhangDepth) {
            return false;
        }
    }
    return true;
}

bool ConstraintChecker::fulfillsOverhangJumpable(Vector newPoint) const {
    floatType height = 0;
    for (int index = 0; index < this->groundPolyRepresentation->getPoints().size() - 1; index++) {
        Vector start = this->groundPolyRepresentation->getPoints().at(index);
        Vector end = this->groundPolyRepresentation->getPoints().at(index + 1);
        Vector delta = end - start;
        if (delta.x <= 0 || delta.computeSlope() >= this->hikerConstants.maxClimbableSlope) {
            height += delta.y;
        } else {
            height = 0;
        }
        if (fabs(height) > this->terrainConstants.maximalOverhangHeight) {
            return false;
        }
    }
    return true;
}

bool ConstraintChecker::fulfillsMinimalBasePointDistance(const std::shared_ptr<StaticPolyline> &additionalTerrain,
                                                         Vector newPoint) const {
    for (Vector point : this->groundPolyRepresentation->getPoints()) {
        if (point.distanceTo(newPoint) < this->terrainConstants.minimalBasePointDistance) {
            return false;
        }
        floatType multiplier = this->terrainConstants.minimalBasePointDistance /
                               static_cast<floatType>(additionalTerrain->getPoints().size() - 1);
        for (int index = 1; index < additionalTerrain->getPoints().size(); index++) {
            Vector additionalPoint = additionalTerrain->getPoints().at(index);
            if (point.distanceTo(additionalPoint) < multiplier * static_cast<floatType>(index)) {
                return false;
            }
        }
    }
    return true;
}

bool ConstraintChecker::fulfillsMinimalBasePolylineAngle(Vector newPoint) const {
    if (this->groundBasePoints->getPoints().size() < 2) {
        return true;
    }
    Vector lastDelta = this->groundBasePoints->getPoints().at(this->groundBasePoints->getPoints().size() - 2) -
                       this->groundBasePoints->getEndPoint();
    Vector newDelta = newPoint - this->groundBasePoints->getEndPoint();
    if (fabs(Vector::getRelativeAngle(lastDelta, newDelta)) < this->terrainConstants.minimalBasePolylineAngle) {
        return false;
    }
    return true;
}

bool ConstraintChecker::isOutsideHikerClearance(Vector oldPoint, Vector newPoint, Vector direction) const {
    if (direction.x > 0 || (direction.x == 0 && direction.y < 0)) {
        if (newPoint.x > oldPoint.x &&
            newPoint.x <
                oldPoint.x + this->hikerConstants.hikerWidth * this->terrainConstants.hikerClearanceTolerance &&
            newPoint.y > oldPoint.y &&
            newPoint.y <
                oldPoint.y + this->hikerConstants.hikerHeight * this->terrainConstants.hikerClearanceTolerance) {
            return false;
        }
    } else {
        if (newPoint.x < oldPoint.x &&
            newPoint.x >
                oldPoint.x - this->hikerConstants.hikerWidth * this->terrainConstants.hikerClearanceTolerance &&
            newPoint.y < oldPoint.y &&
            newPoint.y >
                oldPoint.y - this->hikerConstants.hikerHeight * this->terrainConstants.hikerClearanceTolerance) {
            return false;
        }
    }
    return true;
}

bool ConstraintChecker::isOutsideHikerClearance(Vector oldPoint,
                                                const std::shared_ptr<StaticPolyline> &newTerrain,
                                                Vector direction) const {
    return std::all_of(newTerrain->getPoints().begin(), newTerrain->getPoints().end(), [&](const Vector &newPoint) {
        return this->isOutsideHikerClearance(oldPoint, newPoint, direction);
    });
}

bool ConstraintChecker::isOutsideHikerClearanceAnyDirection(Vector oldPoint, Vector newPoint) const {
    if (newPoint.x > oldPoint.x - this->hikerConstants.hikerWidth * this->terrainConstants.hikerClearanceTolerance &&
        newPoint.x < oldPoint.x + this->hikerConstants.hikerWidth * this->terrainConstants.hikerClearanceTolerance &&
        newPoint.y > oldPoint.y - this->hikerConstants.hikerHeight * this->terrainConstants.hikerClearanceTolerance &&
        newPoint.y < oldPoint.y + this->hikerConstants.hikerHeight * this->terrainConstants.hikerClearanceTolerance) {
        return false;
    }
    return true;
}

bool ConstraintChecker::fulfillsOverhangConstraints(Vector newPoint) const {
    return this->fulfillsMaximumOverhangDepth(newPoint) && this->fulfillsOverhangJumpable(newPoint);
}

bool ConstraintChecker::exceedsMinimalXPos(Vector newPoint) const { return newPoint.x >= this->minXPos; }
