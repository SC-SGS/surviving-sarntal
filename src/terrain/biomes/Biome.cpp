//
// Created by felix-roehr on 7/25/24.
//

#include "Biome.hpp"

#include "../../output/graphics/Renderer.h"
#include "../../utilities/RandomGenerator.hpp"
#include "../generation/GroundGenerator.hpp"
#include <optional>
#include <utility>

Biome::Biome(const std::vector<TerrainPhase> &phaseChoices,
             const TerrainPhase &firstPhase,
             Vector &basePoint,
             Vector &baseDerivative,
             floatType startT,
             floatType length,
             HikerConstants &hikerConstants,
             TerrainConstants &terrainConstants,
             ResourceManager &resourceManager,
             bool renderGeneration)
    : phaseChoices(phaseChoices),
      firstPhase(firstPhase),
      leftBorder(basePoint.x),
      rightBorder(basePoint.x + length),
      hikerConstants(hikerConstants),
      terrainConstants(terrainConstants) {
    auto groundGenerator = GroundGenerator(this->phaseChoices, this->firstPhase, basePoint, baseDerivative, startT,
                                           length, hikerConstants, terrainConstants, resourceManager, renderGeneration);
    this->ground = groundGenerator.generateGround();
    this->rightBorder = this->ground->getBasePoints()->getEndPoint().x;
    this->updatePolyRepresentations();
    this->computeBoundingBox();
}

TerrainPhase Biome::getRandomPhase() const {
    return this->phaseChoices.at(
        RandomGenerator::getInstance().getRandomNumber(0, static_cast<int>(this->phaseChoices.size() - 1)));
}

std::vector<std::shared_ptr<StaticPolygon>>
Biome::calculatePolyRepresentationOfAllComponents(floatType resolution) const {
    std::vector<std::shared_ptr<StaticPolygon>> components{};
    components.reserve(this->components.size());
    for (const auto &component : this->components) {
        components.push_back(component->calculatePolyRepresentation(resolution));
    }
    return components;
}

std::shared_ptr<StaticPolyline> Biome::calculatePolyRepresentationOfGround(floatType resolution) const {
    return this->ground->calculatePolyRepresentation(resolution);
}

void Biome::updatePolyRepresentations() {
    this->groundRendering = this->calculatePolyRepresentationOfGround(this->terrainConstants.renderingResolution);
    this->componentsRendering =
        this->calculatePolyRepresentationOfAllComponents(this->terrainConstants.renderingResolution);

    this->groundCollisionDetection =
        this->calculateCollisionDetectionRepresentationGround(this->terrainConstants.collisionDetectionResolution);
    this->componentsCollisionDetection =
        this->calculateCollisionDetectionRepresentationComponents(this->terrainConstants.collisionDetectionResolution);
}

std::vector<std::shared_ptr<Intersection>> Biome::getAllIntersections(Line &line) {
    assert(this->boundingBox.intersectsOrIsIn(line));
    std::vector<std::shared_ptr<Intersection>> intersections = {};

    if (this->groundCollisionDetection->getBoundingBox().intersectsOrIsIn(line)) {
        std::vector<std::shared_ptr<Intersection>> additionalIntersections =
            this->groundCollisionDetection->calculateIntersections(line);
        intersections.insert(intersections.cend(), additionalIntersections.cbegin(), additionalIntersections.cend());
    }
    for (const auto &component : this->componentsCollisionDetection) {
        if (component->getBoundingBox().intersectsOrIsIn(line)) {
            std::vector<std::shared_ptr<Intersection>> additionalIntersections =
                component->calculateIntersections(line);
            intersections.insert(intersections.end(), additionalIntersections.cbegin(), additionalIntersections.cend());
        }
    }
    return intersections;
}

const std::shared_ptr<StaticPolyline> &Biome::getGroundRendering() const { return this->groundRendering; }
const std::vector<std::shared_ptr<StaticPolygon>> &Biome::getComponentsRendering() const {
    return this->componentsRendering;
}
const std::shared_ptr<CollisionDetectionRepresentation> &Biome::getGroundCollisionDetection() const {
    return this->groundCollisionDetection;
}
const std::vector<std::shared_ptr<CollisionDetectionRepresentation>> &Biome::getComponentsCollisionDetection() const {
    return this->componentsCollisionDetection;
}

floatType Biome::getLeftBorder() { return this->ground->getBasePoints()->getStartPoint().x; }

floatType Biome::getRightBorder() { return this->ground->getBasePoints()->getEndPoint().x; }

const AxisAlignedBoundingBox &Biome::getBoundingBox() const { return boundingBox; }

void Biome::computeBoundingBox() {
    AxisAlignedBoundingBox newBoundingBox = this->groundRendering->getBoundingBox();
    for (const auto &component : this->componentsRendering) {
        newBoundingBox = newBoundingBox.merge(component->getBoundingBox());
    }
    this->boundingBox = newBoundingBox;
}

const TerrainPhase &Biome::getFirstPhase() const { return firstPhase; }

std::shared_ptr<Ground> Biome::getGround() { return this->ground; }

std::vector<std::shared_ptr<CollisionDetectionRepresentation>>
Biome::calculateCollisionDetectionRepresentationComponents(floatType resolution) const {
    std::vector<std::shared_ptr<CollisionDetectionRepresentation>> components{};
    components.reserve(this->components.size());
    for (const auto &component : this->components) {
        components.push_back(component->calculateCollisionDetectionRepresentation(resolution));
    }
    return components;
}

std::shared_ptr<CollisionDetectionRepresentation>
Biome::calculateCollisionDetectionRepresentationGround(floatType resolution) const {
    return this->ground->calculateCollisionDetectionRepresentation(resolution);
}

Biome::Biome(std::vector<Vector> &groundPoints,
             floatType startT,
             HikerConstants &hikerConstants,
             TerrainConstants &terrainConstants,
             ResourceManager &resourceManager)
    : firstPhase({}),
      leftBorder(groundPoints.front().x),
      rightBorder(groundPoints.back().x),
      hikerConstants(hikerConstants),
      terrainConstants(terrainConstants) {
    std::vector<Vector> derivatives = this->computeDerivatives(groundPoints);
    auto groundLine = std::make_shared<StaticPolyline>(groundPoints);
    this->ground = std::make_shared<Ground>(groundLine, derivatives, startT, this->terrainConstants);
    this->rightBorder = this->ground->getBasePoints()->getEndPoint().x;
    this->updatePolyRepresentations();
    this->computeBoundingBox();
}

std::vector<Vector> Biome::computeDerivatives(const std::vector<Vector> &groundPoints) const {
    assert(groundPoints.size() >= 2);
    std::vector<Vector> derivatives = {groundPoints.at(1) - groundPoints.front()};
    Vector lastPoint = groundPoints.at(0);
    for (int index = 1; index < groundPoints.size(); index++) {
        Vector point = groundPoints.at(index);
        derivatives.push_back(point - lastPoint);
        lastPoint = point;
    }
    return derivatives;
}
