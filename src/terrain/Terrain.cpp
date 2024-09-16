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

std::vector<std::shared_ptr<Intersection>> Terrain::getAllIntersections(Line &line) {
    std::vector<std::shared_ptr<Intersection>> intersections = {};
    bool atLeastOneBiomeIntersects = false;
    for (const auto &biome : this->biomes) {
        if (biome->getBoundingBox().intersectsOrIsIn(line)) {
            std::vector<std::shared_ptr<Intersection>> additionalIntersections = biome->getAllIntersections(line);
            intersections.insert(intersections.end(), additionalIntersections.cbegin(), additionalIntersections.cend());
            atLeastOneBiomeIntersects = true;
        }
    }
    if (!atLeastOneBiomeIntersects) {
        // spdlog::debug("No biome intersects");
    }
    Terrain::sortIntersections(line, intersections);
    return intersections;
}

void Terrain::generateInitial() {
    const Vector basePoint{0, 0};
    const Vector baseDerivative{1, 0};
    const floatType startT = 0;
    auto initialBiome =
        std::make_shared<Biome>(Mountain(basePoint, baseDerivative, startT, this->terrainConstants.biomeWidth,
                                         this->hikerConstants, this->terrainConstants, true));
    this->biomes.push_back(initialBiome);
    this->boundingBox = initialBiome->getBoundingBox();
}

std::shared_ptr<Biome> Terrain::generateBiome(BiomeType biomeType, floatType length) {
    Vector basePoint = this->biomes.back()->getGround()->getBasePoints()->getEndPoint();
    Vector baseDerivative = this->biomes.back()->getGround()->getDerivatives()->back();
    floatType startT = this->biomes.back()->getGround()->getEndT();
    std::shared_ptr<Biome> biome;

    switch (biomeType) {
    case BiomeType::MOUNTAIN:
        biome = std::make_shared<Mountain>(basePoint, baseDerivative, startT, length, this->hikerConstants,
                                           this->terrainConstants, false);
        spdlog::info("New biome of type MOUNTAIN generated. Length: {}",
                     this->biomes.back()->getGround()->getBasePoints()->getEndPoint().x -
                         this->biomes.back()->getGround()->getBasePoints()->getStartPoint().x);
        break;
    default:
        spdlog::debug("No viable Biome type selected.");
    }
    return biome;
}

void Terrain::addBiome(const std::shared_ptr<Biome> &biome) {
    assert(this->biomes.back()->getGround()->getBasePoints()->getEndPoint() ==
           biome->getGround()->getBasePoints()->getStartPoint());
    this->biomes.push_back(biome);
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
    Line intersectionLine = {{xPos, this->boundingBox.minMin.y - 5.0f}, {xPos, this->boundingBox.maxMax.y + 5.0f}};
    std::vector<std::shared_ptr<Intersection>> intersections = this->getAllIntersections(intersectionLine);
    return intersections.front()->intersection.y;
}

floatType Terrain::getMaxHeight(floatType xPos) {
    Line intersectionLine = {{xPos, this->boundingBox.maxMax.y + 5.0f}, {xPos, this->boundingBox.minMin.y - 5.0f}};
    std::vector<std::shared_ptr<Intersection>> intersections = this->getAllIntersections(intersectionLine);
    return intersections.front()->intersection.y;
}

std::vector<std::shared_ptr<StaticPolygon>> Terrain::getPolyRepresentationOfAllComponents(floatType resolution) const {
    std::vector<std::shared_ptr<StaticPolygon>> allComponents = {};
    for (const auto &biome : this->biomes) {
        auto biomeComponents = biome->calculatePolyRepresentationOfAllComponents(resolution);
        allComponents.insert(allComponents.cend(), biomeComponents.cbegin(), biomeComponents.cend());
    }
    return allComponents;
}

std::shared_ptr<StaticPolyline> Terrain::getPolyRepresentationOfGround(floatType resolution) const {
    assert(!this->biomes.empty());
    std::shared_ptr<StaticPolyline> groundRepresentation =
        this->biomes.front()->calculatePolyRepresentationOfGround(resolution);
    for (int index = 1; index < this->biomes.size(); index++) {
        groundRepresentation->addPolyline(*this->biomes.at(index)->calculatePolyRepresentationOfGround(resolution));
    }
    return groundRepresentation;
}

std::vector<std::shared_ptr<StaticPolygon>> Terrain::getPolyRepresentationOfAllComponentsRendering() const {
    std::vector<std::shared_ptr<StaticPolygon>> allComponents = {};
    for (const auto &biome : this->biomes) {
        auto const &biomeComponents = biome->getComponentsRendering();
        allComponents.insert(allComponents.cend(), biomeComponents.cbegin(), biomeComponents.cend());
    }
    return allComponents;
}

std::vector<std::shared_ptr<StaticPolyline>> Terrain::getPolyRepresentationOfGroundRendering() const {
    assert(!this->biomes.empty());
    std::vector<std::shared_ptr<StaticPolyline>> groundRepresentation = {};
    groundRepresentation.reserve(this->biomes.size());
    for (auto const &biome : this->biomes) {
        groundRepresentation.push_back(biome->getGroundRendering());
    }

    // std::shared_ptr<StaticPolyline> groundRepresentation = this->biomes.front()->getGroundRendering();
    // for (int index = 1; index < this->biomes.size(); index++) {
    //     groundRepresentation->addPolyline(*this->biomes.at(index)->getGroundRendering());
    // }
    return groundRepresentation;
}

// std::vector<std::shared_ptr<StaticPolygon>> Terrain::getPolyRepresentationOfAllComponentsCollisionDetection() const {
//     std::vector<std::shared_ptr<StaticPolygon>> allComponents = {};
//     for (auto biome : this->biomes) {
//         auto const &biomeComponents = biome->getComponentsCollisionDetection();
//         allComponents.insert(allComponents.cend(), biomeComponents.cbegin(), biomeComponents.cend());
//     }
//     return allComponents;
// }
//
// StaticPolyline *Terrain::getPolyRepresentationOfGroundCollisionDetection() const {
//     assert(!this->biomes.empty());
//     StaticPolyline *groundRepresentation = this->biomes.front()->getGroundCollisionDetection();
//     for (int index = 1; index < this->biomes.size(); index++) {
//         groundRepresentation->addPolyline(this->biomes.at(index)->getGroundCollisionDetection());
//     }
//     return groundRepresentation;
// }

std::shared_ptr<StaticPolyline> Terrain::getBasepoints() const {
    assert(!this->biomes.empty());
    auto basePoints = std::make_shared<StaticPolyline>(*this->biomes.front()->getGround()->getBasePoints());
    for (int index = 1; index < this->biomes.size(); index++) {
        basePoints->addPolyline(StaticPolyline(*this->biomes.front()->getGround()->getBasePoints()));
    }
    return basePoints;
}

void Terrain::recalculateBoundingBox() {
    AxisAlignedBoundingBox newBoundingBox = this->boundingBox;
    for (const auto &biome : this->biomes) {
        newBoundingBox = newBoundingBox.merge(biome->getBoundingBox());
    }
    this->boundingBox = newBoundingBox;
}

void Terrain::sortIntersections(Line &line, std::vector<std::shared_ptr<Intersection>> &intersections) {
    std::sort(intersections.begin(), intersections.end(),
              [&](const std::shared_ptr<Intersection> &firstIntersection,
                  const std::shared_ptr<Intersection> &secondIntersection) {
                  // Compare squared distances from the start of the line
                  return firstIntersection->intersection.distanceTo(line.start) <
                         secondIntersection->intersection.distanceTo(line.start);
              });
}

floatType Terrain::mapHeightToTerrain(Vector point) {
    Line intersectionLineUp = {point, {point.x, this->boundingBox.maxMax.y + 5.0f}};
    std::vector<std::shared_ptr<Intersection>> intersectionsUp = this->getAllIntersections(intersectionLineUp);

    Line intersectionLineDown = {point, {point.x, this->boundingBox.minMin.y - 5.0f}};
    std::vector<std::shared_ptr<Intersection>> intersectionsDown = this->getAllIntersections(intersectionLineDown);

    if (!intersectionsUp.empty() && !intersectionsDown.empty()) {
        if (intersectionsUp.front()->intersection.yDistance(point) <
            intersectionsDown.front()->intersection.yDistance(point)) {
            return intersectionsUp.front()->intersection.y;
        } else {
            return intersectionsDown.front()->intersection.y;
        }
    } else if (intersectionsUp.empty() &&
               intersectionsDown.empty()) { // No intersection with terrain detected. Leave point at the same height
        return point.y;
    } else if (intersectionsUp.empty()) {
        return intersectionsDown.front()->intersection.y;
    } else {
        return intersectionsUp.front()->intersection.y;
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