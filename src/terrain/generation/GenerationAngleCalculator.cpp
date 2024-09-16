//
// Created by felix-roehr on 9/5/24.
//

#include "GenerationAngleCalculator.hpp"
#include "spdlog/spdlog.h"
GenerationAngleCalculator::GenerationAngleCalculator(const std::shared_ptr<Ground> &ground,
                                                     const std::shared_ptr<StaticPolyline> &groundPolyRepresentation,
                                                     TerrainPhase &phase, const HikerConstants &hikerConstants,
                                                     const TerrainConstants &terrainConstants)
    : minAllowedAngle(-PI * phase.randomness), maxAllowedAngle(PI * phase.randomness), phase(phase), ground(ground),
      lastOldPoint(ground->getBasePoints()->getEndPoint()), groundPolyRepresentation(groundPolyRepresentation),
      constraintChecker(
          std::make_shared<ConstraintChecker>(ground, groundPolyRepresentation, terrainConstants, hikerConstants)) {
    this->deltaMaxAngle = this->maxAllowedAngle;
    this->deltaMinAngle = this->minAllowedAngle;
    this->maxAngle = this->maxAllowedAngle;
    this->minAngle = this->minAllowedAngle;
}

std::optional<floatType> GenerationAngleCalculator::calculateMinAngle() {
    spdlog::info("MIN:");
    int counter = 0;
    while (this->deltaMinAngle > GenerationAngleCalculator::MIN_STEP && this->minAngle >= this->minAllowedAngle &&
           counter < GenerationAngleCalculator::MAX_TRIES) {
        Vector newPoint = this->lastOldPoint + this->phase.avgDirection.rotateByAngle(this->minAngle);
        this->updateMinAngle(newPoint);
        counter++;
    }
    if (this->minAngle >= this->minAllowedAngle) {
        this->minAngle += this->deltaMinAngle;
    } else {
        this->minAngle = this->minAllowedAngle;
    }
    if (counter == GenerationAngleCalculator::MAX_TRIES) {
        return std::nullopt;
    }

    return minAngle;
}

void GenerationAngleCalculator::updateMinAngle(Vector newPoint) {
    Vector newDerivative = newPoint - this->lastOldPoint;
    if (!this->constraintChecker->fulfillsConstraints(
            newPoint,
            newDerivative)) { // scan through angle domain with step size delta
        this->minAngle += this->deltaMinAngle;
        spdlog::info("Does NOT fulfill constraints.");
    } else { // If constraints are fulfilled, lower step size
        this->deltaMinAngle /= 2;
        this->minAngle -= this->deltaMinAngle;
        spdlog::info("({},{}) fulfills constraints.", newPoint.x, newPoint.y);
    }

    // No viable angle found, try again with smaller step size.
    if (this->minAngle >= this->maxAllowedAngle) {
        this->minAngle = this->minAllowedAngle;
        this->deltaMinAngle /= 2;
    }
}

std::optional<floatType> GenerationAngleCalculator::calculateMaxAngle() {
    spdlog::info("MAX:");
    int counter = 0;
    while (this->deltaMaxAngle > GenerationAngleCalculator::MIN_STEP && this->maxAngle <= this->maxAllowedAngle &&
           counter < GenerationAngleCalculator::MAX_TRIES) {
        Vector newPoint = this->lastOldPoint + this->phase.avgDirection.rotateByAngle(this->maxAngle);
        this->updateMaxAngle(newPoint);
        counter++;
    }
    if (this->maxAngle <= this->maxAllowedAngle) {
        this->maxAngle -= this->deltaMaxAngle;
    } else {
        this->maxAngle = this->maxAllowedAngle;
    }
    if (counter == GenerationAngleCalculator::MAX_TRIES) {
        return std::nullopt;
    }

    return this->maxAngle;
}

void GenerationAngleCalculator::updateMaxAngle(Vector newPoint) {
    Vector newDerivative = newPoint - this->lastOldPoint;
    if (!this->constraintChecker->fulfillsConstraints(
            newPoint,
            newDerivative)) { // scan through angle domain with step size delta
        this->maxAngle -= this->deltaMaxAngle;
        spdlog::info("Does NOT fulfill constraints.");
    } else { // If constraints are fulfilled, lower step size
        this->deltaMaxAngle /= 2;
        this->maxAngle += this->deltaMaxAngle;
        spdlog::info("({},{}) fulfills constraints.", newPoint.x, newPoint.y);
    }

    // No viable angle found, try again with smaller step size.
    if (this->maxAngle <= this->minAllowedAngle) {
        this->maxAngle = this->maxAllowedAngle;
        this->deltaMaxAngle /= 2;
    }
}

std::shared_ptr<ConstraintChecker> GenerationAngleCalculator::getConstraintChecker() const { return constraintChecker; }
