//
// Created by felix-roehr on 7/25/24.
//

#include "Terrain.hpp"
#include "biomes/Mountain.hpp"
#include "spdlog/spdlog.h"

Terrain::Terrain(HikerConstants hikerConstants, TerrainConstants terrainConstants)
    : hikerConstants(hikerConstants), terrainConstants(terrainConstants) {
    this->generateInitial();
}

std::vector<Intersection> Terrain::getAllIntersections(Line &line) {
    std::vector<Intersection> intersections = {};
    bool atLeastOneBiomeIntersects = false;
    for (Biome *biome : this->biomes) {
        if (biome->getBoundingBox().intersectsOrIsIn(line)) {
            std::vector<Intersection> additionalIntersections = biome->getAllIntersections(line);
            intersections.insert(intersections.end(), additionalIntersections.cbegin(), additionalIntersections.cend());
            atLeastOneBiomeIntersects = true;
        }
    }
    if (!atLeastOneBiomeIntersects) {
        spdlog::debug("No biome intersects");
    }
    Terrain::sortIntersections(line, intersections);
    return intersections;
}

void Terrain::generateInitial() {
    Vector basePoint{0, 0};
    Vector baseDerivative{1, 0};
    floatType startT = 0;
    Biome *initialBiome = new Mountain(basePoint, baseDerivative, startT, this->terrainConstants.biomeWidth,
                                       this->hikerConstants, this->terrainConstants);
    this->biomes.push_back(initialBiome);
    this->boundingBox = initialBiome->getBoundingBox();
}

void Terrain::generateBiome(const BiomeType newBiome, const floatType length) {
    Vector basePoint = this->biomes.back()->getGround()->getBasePoints()->getEndPoint();
    Vector baseDerivative = this->biomes.back()->getGround()->getDerivatives()->back();
    floatType startT = this->biomes.back()->getGround()->getEndT();
    switch (newBiome) {
    case BiomeType::MOUNTAIN:
        this->biomes.push_back(
            new Mountain(basePoint, baseDerivative, startT, length, this->hikerConstants, this->terrainConstants));
        spdlog::info("New biome of type MOUNTAIN generated. Length: {}",
                     this->biomes.back()->getGround()->getBasePoints()->getEndPoint().x -
                         this->biomes.back()->getGround()->getBasePoints()->getStartPoint().x);
        break;
    default:
        spdlog::debug("No viable Biome type selected.");
    }
    this->recalculateBoundingBox();
}

floatType Terrain::getRightBorder() const { return this->biomes.back()->getGround()->getBasePoints()->getEndPoint().x; }

floatType Terrain::getLeftBorder() const {
    return this->biomes.front()->getGround()->getBasePoints()->getStartPoint().x;
}

floatType Terrain::getMaxHeight() const { return this->boundingBox.maxMax.y; }

floatType Terrain::getMinHeight() const { return this->boundingBox.minMin.y; }

void Terrain::removeBiome(float xPos) {
    int index = 0;
    while (index < this->biomes.size() && this->biomes.at(index)->getBoundingBox().maxMax.x < xPos) {
        index++;
    }
    this->biomes.erase(this->biomes.cbegin(), this->biomes.cbegin() + index);
    this->recalculateBoundingBox();
}

floatType Terrain::getGroundHeight(floatType xPos) {
    Line intersectionLine = {{xPos, this->boundingBox.minMin.y}, {xPos, this->boundingBox.maxMax.y}};
    std::vector<Intersection> intersections = this->getAllIntersections(intersectionLine);
    return intersections.front().intersection.y;
}

floatType Terrain::getMaxHeight(floatType xPos) {
    Line intersectionLine = {{xPos, this->boundingBox.maxMax.y}, {xPos, this->boundingBox.minMin.y}};
    std::vector<Intersection> intersections = this->getAllIntersections(intersectionLine);
    return intersections.front().intersection.y;
}

std::vector<StaticPolygon *> Terrain::getPolyRepresentationOfAllComponents(floatType resolution) const {
    std::vector<StaticPolygon *> allComponents = {};
    for (Biome *biome : this->biomes) {
        auto biomeComponents = biome->calculatePolyRepresentationOfAllComponents(resolution);
        allComponents.insert(allComponents.cend(), biomeComponents.cbegin(), biomeComponents.cend());
    }
    return allComponents;
}

StaticPolyline *Terrain::getPolyRepresentationOfGround(floatType resolution) const {
    assert(!this->biomes.empty());
    StaticPolyline *groundRepresentation = this->biomes.front()->calculatePolyRepresentationOfGround(resolution);
    for (int index = 1; index < this->biomes.size(); index++) {
        groundRepresentation->addPolyline(this->biomes.at(index)->calculatePolyRepresentationOfGround(resolution));
    }
    return groundRepresentation;
}

std::vector<StaticPolygon *> Terrain::getPolyRepresentationOfAllComponentsRendering() const {
    std::vector<StaticPolygon *> allComponents = {};
    for (Biome *biome : this->biomes) {
        auto biomeComponents = biome->getComponentsRendering();
        allComponents.insert(allComponents.cend(), biomeComponents.cbegin(), biomeComponents.cend());
    }
    return allComponents;
}

StaticPolyline *Terrain::getPolyRepresentationOfGroundRendering() const {
    assert(!this->biomes.empty());
    StaticPolyline *groundRepresentation = this->biomes.front()->getGroundRendering();
    for (int index = 1; index < this->biomes.size(); index++) {
        groundRepresentation->addPolyline(this->biomes.at(index)->getGroundRendering());
    }
    return groundRepresentation;
}

std::vector<StaticPolygon *> Terrain::getPolyRepresentationOfAllComponentsCollisionDetection() const {
    std::vector<StaticPolygon *> allComponents = {};
    for (Biome *biome : this->biomes) {
        auto biomeComponents = biome->getComponentsCollisionDetection();
        allComponents.insert(allComponents.cend(), biomeComponents.cbegin(), biomeComponents.cend());
    }
    return allComponents;
}

StaticPolyline *Terrain::getPolyRepresentationOfGroundCollisionDetection() const {
    assert(!this->biomes.empty());
    StaticPolyline *groundRepresentation = this->biomes.front()->getGroundCollisionDetection();
    for (int index = 1; index < this->biomes.size(); index++) {
        groundRepresentation->addPolyline(this->biomes.at(index)->getGroundCollisionDetection());
    }
    return groundRepresentation;
}

StaticPolyline *Terrain::getBasepoints() const {
    assert(!this->biomes.empty());
    auto basePoints = new StaticPolyline(*this->biomes.front()->getGround()->getBasePoints());
    for (int index = 1; index < this->biomes.size(); index++) {
        basePoints->addPolyline(new StaticPolyline(*this->biomes.front()->getGround()->getBasePoints()));
    }
    return basePoints;
}

void Terrain::recalculateBoundingBox() {
    AxisAlignedBoundingBox newBoundingBox = this->boundingBox;
    for (auto biome : this->biomes) {
        newBoundingBox = newBoundingBox.merge(biome->getBoundingBox());
    }
}

void Terrain::sortIntersections(Line &line, std::vector<Intersection> &intersections) {
    std::sort(intersections.begin(), intersections.end(),
              [&](const Intersection &firstIntersection, const Intersection &secondIntersection) {
                  // Compare squared distances from the start of the line
                  return firstIntersection.intersection.distanceTo(line.start) <
                         secondIntersection.intersection.distanceTo(line.start);
              });
}

floatType Terrain::mapHeightToTerrain(Vector point) {
    Line intersectionLineUp = {point, {point.x, this->boundingBox.maxMax.y}};
    std::vector<Intersection> intersectionsUp = this->getAllIntersections(intersectionLineUp);

    Line intersectionLineDown = {point, {point.x, this->boundingBox.minMin.y}};
    std::vector<Intersection> intersectionsDown = this->getAllIntersections(intersectionLineDown);

    if (!intersectionsUp.empty() && !intersectionsDown.empty()) {
        if (intersectionsUp.front().intersection.yDistance(point) <
            intersectionsDown.front().intersection.yDistance(point)) {
            return intersectionsUp.front().intersection.y;
        } else {
            return intersectionsDown.front().intersection.y;
        }
    } else if (intersectionsUp.empty()) {
        return intersectionsDown.front().intersection.y;
    } else {
        return intersectionsUp.front().intersection.y;
    }
}

Terrain Terrain::getEmptyTerrain(HikerConstants hikerConstants, TerrainConstants terrainConstants,
                                 AxisAlignedBoundingBox boundingBox) {
    return {hikerConstants, terrainConstants, boundingBox};
}

Terrain::Terrain(HikerConstants hikerConstants, TerrainConstants terrainConstants, AxisAlignedBoundingBox boundingBox)
    : hikerConstants(hikerConstants), terrainConstants(terrainConstants) {
    this->boundingBox = boundingBox;
}
