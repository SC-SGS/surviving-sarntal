//
// Created by felix-roehr on 7/25/24.
//

#include "Biome.hpp"

#include "../../output/graphics/Renderer.h"
#include "../../utilities/RandomGenerator.hpp"
#include <optional>
#include <utility>

Biome::Biome(const std::vector<TerrainPhase> &phaseChoices, Vector basePoint, Vector baseDerivative, floatType startT,
             floatType length, HikerConstants hikerConstants, TerrainConstants terrainConstants)
    : phaseChoices(phaseChoices), leftBorder(basePoint.x), rightBorder(basePoint.x + length),
      hikerConstants(hikerConstants), terrainConstants(terrainConstants) {
    this->ground = this->generateGround(basePoint, baseDerivative, startT);
    // std::vector<Vector> groundDerivatives = Biome::generateSemiSmoothDerivatives(groundPoints, 0.2f);
    // groundDerivatives.at(0) = baseDerivative;
    // this->ground = new Ground(new Polyline(groundPoints), &groundDerivatives, startT);
    this->rightBorder = this->ground->getBasePoints()->getEndPoint().x;
    this->updatePolyRepresentations();
    this->computeBoundingBox();
}

TerrainPhase Biome::getRandomPhase() const {
    return this->phaseChoices.at(
        RandomGenerator::getInstance().getRandomNumber(0, static_cast<int>(this->phaseChoices.size() - 1)));
}

Vector Biome::generateNextPoint(TerrainPhase phase, Vector basePoint) const {
    Vector deltaVector = phase.avgDirection.randomRotation(phase.randomness);
    assert(deltaVector.length() == phase.avgDirection.length());
    return basePoint + deltaVector;
}

Vector Biome::generateNextPointContinuous(TerrainPhase phase, std::vector<Vector> oldPoints, int nSmoothness) const {
    Vector deltaVector = phase.avgDirection.randomRotation(phase.randomness);
    int counter = 0;
    while (counter < oldPoints.size() - 1 && counter < nSmoothness) {
        int index = static_cast<int>(oldPoints.size()) - 2 - counter;
        counter++;
        Vector oldDelta = oldPoints.at(index + 1) - oldPoints.at(index);
        floatType relativeAngle = Vector::getRelativeAngle(deltaVector, oldDelta);
        deltaVector.rotateByAngle(relativeAngle / static_cast<floatType>(std::pow(2, static_cast<floatType>(counter))));
    }
    return oldPoints.back() + deltaVector;
}

Ground *Biome::generateGroundBase(Vector basePoint, Vector baseDerivative, floatType startT, TerrainPhase phase) const {
    Vector secondPoint = basePoint + phase.avgDirection;
    std::vector<Vector> firstTwoPoints = {basePoint, secondPoint};
    std::vector<Vector> firstTwoDerivatives = {baseDerivative, phase.avgDirection};
    auto pol = StaticPolyline(firstTwoPoints);
    auto *newGround = new Ground(&pol, &firstTwoDerivatives, startT, this->terrainConstants);
    return newGround;
}

Ground *Biome::generateGround(Vector basePoint, // NOLINT [readability-function,-size-warnings-as-errors]
                              Vector baseDerivative, floatType startT) const {
    TerrainPhase phase = this->getRandomPhase();
    Ground *newGround = this->generateGroundBase(basePoint, baseDerivative, startT, phase);
    Vector oldPoint = newGround->getBasePoints()->getPoints().back();
    floatType xPos = oldPoint.x;
    int phasePointCount = 0;
    bool retrace = false;
    int retraceCount = 0;
    int retracedSteps = 0;
    while (xPos < this->rightBorder) {
        if (retrace) {
            retracedSteps++;
        }
        if (phasePointCount == phase.pointCount) {
            phase = this->getRandomPhase();
            phasePointCount = 0;
        }
        const StaticPolyline *groundPolyRepresentation = newGround->getPolyRepresentationForGeneration();
        std::optional<floatType> minAngle = this->getMinGenerationAngle(phase, newGround, groundPolyRepresentation);
        std::optional<floatType> maxAngle = this->getMaxGenerationAngle(phase, newGround, groundPolyRepresentation);
        if (!minAngle.has_value() || !maxAngle.has_value()) {
            retraceCount += 2;
            newGround->removeLastBasepoints(retraceCount);
            oldPoint = newGround->getBasePoints()->getEndPoint();
            xPos = oldPoint.x;
            retrace = true;
            retracedSteps = 0;
            continue;
        }
        if (retracedSteps > retraceCount) {
            retrace = false;
            retraceCount = 0;
            retracedSteps = 0;
        }

        Vector newPoint{};
        Vector newDerivative{};
        do {
            newPoint = this->generateNextPoint(phase, oldPoint, minAngle.value(), maxAngle.value());
            newDerivative = newPoint - oldPoint;
        } while (!this->fulfillsConstraints(newPoint, newDerivative, newGround, groundPolyRepresentation));

        xPos = newPoint.x;
        newGround->addTerrain(newPoint, newDerivative);
        phasePointCount++;
        oldPoint = newPoint;

        this->generationRendering(newGround, groundPolyRepresentation, oldPoint, newPoint, phase, minAngle.value(),
                                  maxAngle.value());
    }
    return newGround;
}

std::vector<StaticPolygon *> Biome::calculatePolyRepresentationOfAllComponents(floatType resolution) const {
    std::vector<StaticPolygon *> components{};
    components.reserve(this->components.size());
    for (auto component : this->components) {
        components.push_back(component->calculatePolyRepresentation(resolution));
    }
    return components;
}

StaticPolyline *Biome::calculatePolyRepresentationOfGround(floatType resolution) const {
    return this->ground->calculatePolyRepresentation(resolution);
}

void Biome::updatePolyRepresentations() {
    this->groundRendering = this->calculatePolyRepresentationOfGround(this->terrainConstants.renderingResolution);
    this->componentsRendering =
        this->calculatePolyRepresentationOfAllComponents(this->terrainConstants.renderingResolution);

    this->groundCollisionDetection =
        this->calculatePolyRepresentationOfGround(this->terrainConstants.collisionDetectionResolution);
    this->componentsCollisionDetection =
        this->calculatePolyRepresentationOfAllComponents(this->terrainConstants.collisionDetectionResolution);
}

std::vector<Vector> Biome::generateSemiSmoothDerivatives(std::vector<Vector> points, floatType randomness) {
    assert(points.size() >= 2);
    std::vector<Vector> derivatives = {};
    derivatives.push_back((points.at(1) - points.at(0)).randomRotation(randomness));
    for (int index = 1; index < points.size() - 1; index++) {
        derivatives.push_back(points.at(index + 1) - points.at(index - 1).randomRotation(randomness));
    }
    derivatives.push_back(points.at(points.size() - 1) - points.at(points.size() - 2).randomRotation(randomness));
    return derivatives;
}

std::vector<Vector> Biome::generateSmoothDerivatives(std::vector<Vector> points) {
    return Biome::generateSemiSmoothDerivatives(std::move(points), 0);
}

Ground *Biome::getGround() { return this->ground; }

bool Biome::fulfillsConstraints(Vector newPoint, Vector newDerivative, const Ground *newGround,
                                const StaticPolyline *currentPolyRepresentation) const {
    StaticPolyline *additionalPolyline = newGround->getPolyRepresentationLastPointToNewPoint(
        newPoint, newDerivative, this->terrainConstants.renderingResolution);
    return this->fulfillsHikerClearance(currentPolyRepresentation, additionalPolyline, newPoint) &&
           this->fulfillsMinimalBasePointDistance(currentPolyRepresentation, additionalPolyline, newPoint) &&
           this->fulfillsMinimalBasePolylineAngle(newGround->getBasePoints(), newPoint) &&
           this->fulfillsNoSelfPenetration(currentPolyRepresentation, additionalPolyline) &&
           this->fulfillsMaximumOverhangDepth(currentPolyRepresentation, additionalPolyline) &&
           this->fulfillsOverhangJumpable(currentPolyRepresentation, additionalPolyline);
}

bool Biome::fulfillsHikerClearance(const StaticPolyline *currentPolyRepresentation,
                                   const StaticPolyline *additionalTerrain, Vector newPoint) const {
    Vector lastPointOldTerrain = currentPolyRepresentation->getPoints().back();
    for (int index = 0; index < currentPolyRepresentation->getPoints().size() - 1; index++) {
        Vector prevPoint = currentPolyRepresentation->getPoints().at(index);
        Vector currentPoint = currentPolyRepresentation->getPoints().at(index + 1);
        Vector direction = currentPoint - prevPoint;
        if (this->isOutsideHikerClearanceAnyDirection(currentPoint, lastPointOldTerrain)) {
            if (!this->isOutsideHikerClearance(prevPoint, additionalTerrain, direction)) {
                return false;
            }
        }
    }
    return true;
}

bool Biome::fulfillsNoSelfPenetration(const StaticPolyline *currentPolyRepresentation,
                                      StaticPolyline *additionalTerrain) const {
    return true;
    return !currentPolyRepresentation->intersectsWithoutFirstPoint(additionalTerrain);
}

bool Biome::fulfillsMaximumOverhangDepth(const StaticPolyline *currentPolyRepresentation,
                                         StaticPolyline *additionalTerrain) const {
    return true;
}

bool Biome::fulfillsOverhangJumpable(const StaticPolyline *currentPolyRepresentation,
                                     StaticPolyline *additionalTerrain) const {
    return true;
}

bool Biome::placeStructure(AxisAlignedBoundingBox spacelimit, Structure *structure) { return false; }
void Biome::placeStructures() {}

std::vector<Intersection> Biome::getAllIntersections(Line &line) {
    assert(this->boundingBox.intersectsOrIsIn(line));
    std::vector<Intersection> intersections = {};
    if (this->groundCollisionDetection->getBoundingBox().intersectsOrIsIn(line)) {
        std::vector<Intersection> additionalIntersections =
            this->groundCollisionDetection->calculateIntersections(line);
        intersections.insert(intersections.end(), additionalIntersections.cbegin(), additionalIntersections.cend());
    }
    for (auto component : this->componentsCollisionDetection) {
        if (component->getBoundingBox().intersectsOrIsIn(line)) {
            std::vector<Intersection> additionalIntersections = component->calculateIntersections(line);
            intersections.insert(intersections.end(), additionalIntersections.cbegin(), additionalIntersections.cend());
        }
    }
    return intersections;
}

StaticPolyline *Biome::getGroundRendering() const { return groundRendering; }
const std::vector<StaticPolygon *> &Biome::getComponentsRendering() const { return componentsRendering; }
StaticPolyline *Biome::getGroundCollisionDetection() const { return groundCollisionDetection; }
const std::vector<StaticPolygon *> &Biome::getComponentsCollisionDetection() const {
    return componentsCollisionDetection;
}

std::optional<floatType> Biome::getMinGenerationAngle( // NOLINT [readability-function-size,-warnings-as-errors]
    TerrainPhase phase, Ground *newGround, const StaticPolyline *groundPolyRepresentation) const {
    spdlog::info("MIN:");
    floatType minStep = 0.01f;
    floatType minAngle = -PI * phase.randomness;
    auto delta = PI * phase.randomness;
    int counter = 0;
    while (delta > minStep && minAngle >= -PI * phase.randomness && counter < 30) {
        Vector oldPoint = newGround->getBasePoints()->getEndPoint();
        Vector newPoint = oldPoint + phase.avgDirection.rotateByAngle(minAngle);
        Vector newDerivative = newPoint - oldPoint;
        if (!this->fulfillsConstraints(newPoint, newDerivative, newGround,
                                       groundPolyRepresentation)) { // scan through angle domain with step size delta
            minAngle += delta;
            spdlog::info("Does NOT fulfill constraints.");
        } else { // If constraints are fulfilled, lower step size
            delta /= 2;
            minAngle -= delta;
            assert(this->fulfillsConstraints(newPoint, newDerivative, newGround, groundPolyRepresentation));
            spdlog::info("({},{}) fulfills constraints.", newPoint.x, newPoint.y);
        }

        // No viable angle found, try again with smaller step size.
        if (minAngle >= PI * phase.randomness) {
            minAngle = -PI * phase.randomness;
            delta /= 2;
        }
        counter++;
    }
    if (minAngle >= -PI * phase.randomness) {
        minAngle += delta;
    } else {
        minAngle = -PI * phase.randomness;
    }

    if (counter == 30) {
        return std::nullopt;
    }

    Vector newPoint = newGround->getBasePoints()->getEndPoint() + phase.avgDirection.rotateByAngle(minAngle);
    spdlog::info("({},{}) created.", newPoint.x, newPoint.y);
    return minAngle;
}

std::optional<floatType> Biome::getMaxGenerationAngle( // NOLINT [readability-function-size,-warnings-as-errors]
    TerrainPhase phase, Ground *newGround, const StaticPolyline *groundPolyRepresentation) const {
    spdlog::info("MAX:");
    floatType minStep = 0.01f;
    floatType maxAngle = PI * phase.randomness;
    auto delta = PI * phase.randomness;
    int counter = 0;
    while (delta > minStep && maxAngle <= PI * phase.randomness && counter < 30) {
        Vector oldPoint = newGround->getBasePoints()->getEndPoint();
        Vector newPoint = oldPoint + phase.avgDirection.rotateByAngle(maxAngle);
        Vector newDerivative = newPoint - oldPoint;
        if (!this->fulfillsConstraints(newPoint, newDerivative, newGround,
                                       groundPolyRepresentation)) { // scan through angle domain with step size delta
            maxAngle -= delta;
            spdlog::info("Does NOT fulfill constraints.");
        } else { // If constraints are fulfilled, lower step size
            delta /= 2;
            maxAngle += delta;
            assert(this->fulfillsConstraints(newPoint, newDerivative, newGround, groundPolyRepresentation));
            spdlog::info("({},{}) fulfills constraints.", newPoint.x, newPoint.y);
        }

        // No viable angle found, try again with smaller step size.
        if (maxAngle <= -PI * phase.randomness) {
            maxAngle = PI * phase.randomness;
            delta /= 2;
        }

        counter++;
    }
    if (maxAngle <= PI * phase.randomness) {
        maxAngle -= delta;
    } else {
        maxAngle = PI * phase.randomness;
    }
    if (counter == 30) {
        return std::nullopt;
    }

    Vector newPoint = newGround->getBasePoints()->getEndPoint() + phase.avgDirection.rotateByAngle(maxAngle);
    spdlog::info("({},{}) created.", newPoint.x, newPoint.y);
    return maxAngle;
}

Vector Biome::generateNextPoint(TerrainPhase phase, Vector basePoint, floatType minAngle, floatType maxAngle) const {
    return basePoint + phase.avgDirection.rotateByRandomAngle(minAngle, maxAngle);
}

bool Biome::fulfillsMinimalBasePointDistance(const StaticPolyline *currentPolyRepresentation,
                                             const StaticPolyline *additionalTerrain, Vector newPoint) const {
    for (Vector point : currentPolyRepresentation->getPoints()) {
        if (point.distanceTo(newPoint) < this->terrainConstants.minimalBasePointDistance) {
            return false;
        }
        floatType multiplier = this->terrainConstants.minimalBasePointDistance /
                               static_cast<floatType>(additionalTerrain->getPoints().size() - 1);
        for (int index = 1; index < additionalTerrain->getPoints().size(); index++) {
            Vector newPoint = additionalTerrain->getPoints().at(index);
            if (point.distanceTo(newPoint) < multiplier * static_cast<floatType>(index)) {
                return false;
            }
        }
    }
    return true;
}

bool Biome::fulfillsMinimalBasePolylineAngle(const StaticPolyline *currentBasePoints, Vector newPoint) const {
    if (currentBasePoints->getPoints().size() < 2) {
        return true;
    }
    Vector lastDelta =
        currentBasePoints->getPoints().at(currentBasePoints->getPoints().size() - 2) - currentBasePoints->getEndPoint();
    Vector newDelta = newPoint - currentBasePoints->getEndPoint();
    if (fabs(Vector::getRelativeAngle(lastDelta, newDelta)) < this->terrainConstants.minimalBasePolylineAngle) {
        return false;
    }
    return true;
}

Camera2D Biome::setUpGenerationRendering() const {
    const floatType leftBorder = -0.02f * this->terrainConstants.biomeWidth;
    const floatType rightBorder = 1.02f * this->terrainConstants.biomeWidth;

    // Calculate the world width
    const floatType worldWidth = rightBorder - leftBorder;

    // Calculate the zoom level so that the world width fits the screen width
    const auto screenWidth = static_cast<floatType>(GetScreenWidth());
    const floatType zoom = screenWidth / worldWidth;

    // Calculate the visible width and height
    const floatType visibleWidth = rightBorder - leftBorder;
    const floatType visibleHeight =
        visibleWidth * (static_cast<floatType>(GetScreenHeight()) / static_cast<floatType>(GetScreenWidth()));

    // Calculate the center of the camera view based on the borders
    const floatType centerX = (leftBorder + rightBorder) / 2.0f;
    const floatType centerY = 0;

    const Vector2 screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};

    Camera2D camera;
    camera.target = {centerX, centerY};
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = zoom;

    return camera;
}
Vector2 Biome::transformPosition(Vector position) {
    floatType xValue = position.x;
    floatType yValue = -position.y;
    return {xValue, yValue};
    // return {position.x, -position.y + static_cast<floatType>(GetScreenHeight())/camera.zoom};
}

bool Biome::isOutsideHikerClearance(Vector oldPoint, Vector newPoint, Vector direction) const {
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

bool Biome::isOutsideHikerClearance(Vector oldPoint, const StaticPolyline *newTerrain, Vector direction) const {
    return std::all_of(newTerrain->getPoints().begin(), newTerrain->getPoints().end(), [&](const Vector &newPoint) {
        return this->isOutsideHikerClearance(oldPoint, newPoint, direction);
    });
}

bool Biome::isOutsideHikerClearanceAnyDirection(Vector oldPoint, Vector newPoint) const {
    if (newPoint.x > oldPoint.x - this->hikerConstants.hikerWidth * this->terrainConstants.hikerClearanceTolerance &&
        newPoint.x < oldPoint.x + this->hikerConstants.hikerWidth * this->terrainConstants.hikerClearanceTolerance &&
        newPoint.y > oldPoint.y - this->hikerConstants.hikerHeight * this->terrainConstants.hikerClearanceTolerance &&
        newPoint.y < oldPoint.y + this->hikerConstants.hikerHeight * this->terrainConstants.hikerClearanceTolerance) {
        return false;
    }
    return true;
}

floatType Biome::getLeftBorder() { return this->ground->getBasePoints()->getStartPoint().x; }

floatType Biome::getRightBorder() { return this->ground->getBasePoints()->getEndPoint().x; }

// NOLINTBEGIN
void Biome::generationRendering(Ground *newGround, const StaticPolyline *groundPolyRepresentation, Vector oldPoint,
                                Vector newPoint, TerrainPhase phase, floatType minAngle, floatType maxAngle) const {
    Camera2D camera = this->setUpGenerationRendering();
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);
    Vector delta = {0.5f, 0.5f};
    Vector2 basePointRendering = Biome::transformPosition(newGround->getBasePoints()->getStartPoint() + delta);
    DrawRectangle(basePointRendering.x, basePointRendering.y - this->hikerConstants.hikerHeight,
                  this->hikerConstants.hikerWidth, this->hikerConstants.hikerHeight, GREEN);
    rlBegin(RL_LINES);
    rlColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < groundPolyRepresentation->getPoints().size() - 2; i++) {
        Vector point = groundPolyRepresentation->getPoints().at(i);
        Vector nextPoint = groundPolyRepresentation->getPoints().at(i + 1);

        Vector2 renderingPoint = Biome::transformPosition(point);
        Vector2 renderingToPoint = Biome::transformPosition(nextPoint);

        rlVertex2f(renderingPoint.x, renderingPoint.y);
        rlVertex2f(renderingToPoint.x, renderingToPoint.y);
    }
    rlEnd();
    StaticPolyline const *basePoints = newGround->getBasePoints();
    for (Vector point : basePoints->getPoints()) {
        Vector2 renderingPoint = Biome::transformPosition(point);
        DrawCircle(static_cast<int>(renderingPoint.x), static_cast<int>(renderingPoint.y), 0.07f, GREEN);
    }
    Vector minPoint = oldPoint + phase.avgDirection.rotateByAngle(minAngle);
    Vector maxPoint = oldPoint + phase.avgDirection.rotateByAngle(maxAngle);
    Vector2 oldPointRendering = Biome::transformPosition(oldPoint);
    Vector2 minPointRendering = Biome::transformPosition(minPoint);
    Vector2 maxPointRendering = Biome::transformPosition(maxPoint);
    DrawLineEx(oldPointRendering, minPointRendering, 0.04f, BROWN);
    DrawLineEx(oldPointRendering, maxPointRendering, 0.04f, SKYBLUE);
    Vector2 newPointRendering = Biome::transformPosition(newPoint);
    DrawLineEx(oldPointRendering, newPointRendering, 0.05f, ORANGE);
    EndMode2D();
    EndDrawing();
}
// NOLINTEND

const AxisAlignedBoundingBox &Biome::getBoundingBox() const { return boundingBox; }

void Biome::computeBoundingBox() {
    AxisAlignedBoundingBox newBoundingBox = this->groundRendering->getBoundingBox();
    for (auto component : this->componentsRendering) {
        newBoundingBox = newBoundingBox.merge(component->getBoundingBox());
    }
    this->boundingBox = newBoundingBox;
}
