//
// Created by felix-roehr on 9/4/24.
//

#include "GroundGenerator.hpp"
#include "../../utilities/RandomGenerator.hpp"
#include "../../utilities/Singleton.hpp"
#include "GenerationAngleCalculator.hpp"
GroundGenerator::GroundGenerator(const std::vector<TerrainPhase> &phaseChoices, const TerrainPhase &firstPhase,
                                 Vector basePoint, Vector baseDerivative, floatType startT, floatType length,
                                 HikerConstants &hikerConstants, TerrainConstants &terrainConstants,
                                 ResourceManager &resourceManager, bool render)
    : terrainConstants(terrainConstants), hikerConstants(hikerConstants), render(render),
      generationRenderer(std::make_unique<GenerationRenderer>(hikerConstants, terrainConstants, resourceManager)),
      basePoint(basePoint), baseDerivative(baseDerivative), startT(startT), length(length), phaseChoices(phaseChoices),
      firstPhase(firstPhase), terrainPhase(firstPhase), lastPoint(basePoint) {}

std::shared_ptr<Ground> GroundGenerator::generateGround() {
    this->generateGroundBase();
    while (this->lastPoint.x < this->basePoint.x + this->length) {
        this->updateTerrainPhase();
        std::optional<Vector> newPoint = this->generateNewPoint();
        if (!newPoint.has_value()) {
            this->retrace();
        } else {
            Vector newDerivative = newPoint.value() - this->lastPoint;
            this->ground->addTerrain(newPoint.value(), newDerivative);
            if (this->retracing) {
                this->retracedSteps++;
            }
            this->updateRetracing();
            this->phasePointCount++;
            this->lastPoint = newPoint.value();
        }
        if (this->render) {
            this->generationRenderer->render(this->ground, this->renderingGroundPolyRepresentation, this->lastPoint,
                                             this->terrainPhase, this->renderingMinAngle, this->renderingMaxAngle);
        }
    }
    return this->ground;
}

void GroundGenerator::generateGroundBase() {
    TerrainPhase firstPhase = this->firstPhase;
    Vector secondPoint = this->basePoint + firstPhase.avgDirection;
    std::vector<Vector> firstTwoPoints = {this->basePoint, secondPoint};
    std::vector<Vector> firstTwoDerivatives = {this->baseDerivative, firstPhase.avgDirection};
    const auto pol = std::make_shared<StaticPolyline>(firstTwoPoints);
    this->ground = std::make_shared<Ground>(pol, firstTwoDerivatives, this->startT, this->terrainConstants);
    this->lastPoint = this->ground->getBasePoints()->getPoints().back();
    this->minXPos = this->lastPoint.x;
}

void GroundGenerator::retrace() {
    auto maxAllowedRetraceCount =
        static_cast<int>(this->ground->getBasePoints()->getPoints().size() - this->firstPhase.pointCount - 1);
    int theoreticalRetraceCount;
    if (this->retracing && this->retracedSteps <= this->retraceCount) {
        // theoreticalRetraceCount = static_cast<int>(
        //     std::ceil(static_cast<floatType>(this->retraceCount) * GroundGenerator::RETRACE_MULTIPLIER));
        theoreticalRetraceCount = this->retraceCount + GroundGenerator::RETRACE_ADDITION;
    } else {
        theoreticalRetraceCount = GroundGenerator::BASE_RETRACE_COUNT;
    }
    this->retraceCount = std::min(theoreticalRetraceCount, maxAllowedRetraceCount);
    this->retracing = true;
    this->retracedSteps = 0;
    this->ground->removeLastBasepoints(std::min(
        this->retraceCount,
        static_cast<int>(this->ground->getBasePoints()->getPoints().size() - this->firstPhase.pointCount - 1)));
    this->lastPoint = this->ground->getBasePoints()->getEndPoint();
}

std::optional<Vector> GroundGenerator::generateNewPoint() {
    const std::shared_ptr<StaticPolyline> groundPolyRepresentation = this->ground->getPolyRepresentationForGeneration();
    this->renderingGroundPolyRepresentation = this->ground->getPolyRepresentationForGeneration();

    auto angleCalculator = GenerationAngleCalculator(this->ground, groundPolyRepresentation, this->terrainPhase,
                                                     this->hikerConstants, this->terrainConstants);
    std::optional<floatType> minAngle = angleCalculator.calculateMinAngle();
    std::optional<floatType> maxAngle = angleCalculator.calculateMaxAngle();
    if (!minAngle.has_value() || !maxAngle.has_value()) {
        return std::nullopt;
    } else {
        this->renderingMinAngle = minAngle.value();
        this->renderingMaxAngle = maxAngle.value();
        return this->generateNewPoint(minAngle.value(), maxAngle.value(), angleCalculator.getConstraintChecker());
    }
}

std::optional<Vector> GroundGenerator::generateNewPoint(floatType minAngle, floatType maxAngle,
                                                        const std::shared_ptr<ConstraintChecker> &constraintChecker) {
    Vector newPoint{};
    Vector newDerivative{};
    int counter = 0;
    do {
        newPoint = this->lastPoint + this->terrainPhase.avgDirection.rotateByRandomAngle(minAngle, maxAngle);
        newDerivative = newPoint - this->lastPoint;
        counter++;
    } while (!constraintChecker->fulfillsConstraints(newPoint, newDerivative) && counter < GroundGenerator::MAX_TRIES);
    if (counter < GroundGenerator::MAX_TRIES) {
        return newPoint;
    } else {
        return std::nullopt;
    }
}

void GroundGenerator::updateTerrainPhase() {
    if (this->phasePointCount >= this->terrainPhase.pointCount) {
        this->terrainPhase = this->getRandomPhase();
        this->phasePointCount = 0;
    }
}

void GroundGenerator::updateRetracing() {
    if (this->retracedSteps > this->retraceCount) {
        this->retracing = false;
        this->retraceCount = 0;
        this->retracedSteps = 0;
    }
}

TerrainPhase GroundGenerator::getRandomPhase() const {
    return this->phaseChoices.at(
        RandomGenerator::getInstance().getRandomNumber(0, static_cast<int>(this->phaseChoices.size() - 1)));
}
