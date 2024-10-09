//
// Created by felix-roehr on 7/25/24.
//

#include "Terrain.hpp"
#include "biomes/Mountain.hpp"
#include "spdlog/fmt/bundled/chrono.h"
#include "spdlog/spdlog.h"

Terrain::Terrain(HikerConstants &hikerConstants, TerrainConstants &terrainConstants, ResourceManager &resourceManager)
    : hikerConstants(hikerConstants), terrainConstants(terrainConstants), resourceManager(resourceManager) {
    // this->generateInitial();
}

/*
Terrain::Terrain(std::vector<Vector> &groundPoints,
                 HikerConstants &hikerConstants,
                 TerrainConstants &terrainConstants,
                 ResourceManager &resourceManager)
    : hikerConstants(hikerConstants), terrainConstants(terrainConstants), resourceManager(resourceManager) {
    this->biomes = {
        std::make_shared<Biome>(Biome(groundPoints, 0.f, hikerConstants, terrainConstants, resourceManager))};
    this->boundingBox = this->biomes.front()->getBoundingBox();
}
*/

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
    sortIntersections(line, intersections);
    return intersections;
}

void Terrain::generateInitial() {
    const Vector basePoint{0, 0};
    const Vector baseDerivative{1, 0};
    const floatType startT = 0;
    const auto initialBiome =
        std::make_shared<Biome>(Mountain(basePoint, baseDerivative, startT, this->terrainConstants.biomeWidth,
                                         this->hikerConstants, this->terrainConstants, this->resourceManager, true));
    this->biomes.push_back(initialBiome);
    this->boundingBox = initialBiome->getBoundingBox();
}

std::shared_ptr<Biome> Terrain::generateBiome(const BiomeType biomeType, const floatType length) const {
    Vector basePoint = this->biomes.back()->getGround()->getBasePoints()->getEndPoint();
    Vector baseDerivative = this->biomes.back()->getGround()->getDerivatives()->back();
    floatType startT = this->biomes.back()->getGround()->getEndT();
    std::shared_ptr<Biome> biome;

    switch (biomeType) {
    case BiomeType::MOUNTAIN:
        biome = std::make_shared<Mountain>(basePoint, baseDerivative, startT, length, this->hikerConstants,
                                           this->terrainConstants, this->resourceManager, false);
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
    spdlog::info("Added biome. Number of biomes: {}", this->biomes.size());
}

floatType Terrain::getRightBorder() const {
    if (this->biomes.empty())
        return 0.f;
    return this->biomes.back()->getGround()->getBasePoints()->getEndPoint().x;
}

floatType Terrain::getLeftBorder() const {
    return this->biomes.front()->getGround()->getBasePoints()->getStartPoint().x;
}

floatType Terrain::getMaxHeight() const { return this->boundingBox.maxMax.y; }

floatType Terrain::getMinHeight() const { return this->boundingBox.minMin.y; }

void Terrain::removeBiome(const float xPos) {
    int index = 0;
    while (index < this->biomes.size() && this->biomes.at(index)->getBoundingBox().maxMax.x < xPos) {
        index++;
    }
    this->biomes.erase(this->biomes.cbegin(), this->biomes.cbegin() + index);
    this->recalculateBoundingBox();
    if (index > 0) {
        spdlog::info("Removed biome. Number of biomes: {}", this->biomes.size());
    }
}

floatType Terrain::getGroundHeight(const floatType xPos) {
    Line intersectionLine = {{xPos, this->boundingBox.minMin.y - 5.0f}, {xPos, this->boundingBox.maxMax.y + 5.0f}};
    const std::vector<std::shared_ptr<Intersection>> intersections = this->getAllIntersections(intersectionLine);
    return intersections.front()->intersection.y;
}

floatType Terrain::getMaxHeight(const floatType xPos) {
    Line intersectionLine = {{xPos, this->boundingBox.maxMax.y + 5.0f}, {xPos, this->boundingBox.minMin.y - 5.0f}};
    const std::vector<std::shared_ptr<Intersection>> intersections = this->getAllIntersections(intersectionLine);
    return intersections.front()->intersection.y;
}

std::vector<std::shared_ptr<StaticPolygon>>
Terrain::getPolyRepresentationOfAllComponents(const floatType resolution) const {
    std::vector<std::shared_ptr<StaticPolygon>> allComponents = {};
    for (const auto &biome : this->biomes) {
        auto biomeComponents = biome->calculatePolyRepresentationOfAllComponents(resolution);
        allComponents.insert(allComponents.cend(), biomeComponents.cbegin(), biomeComponents.cend());
    }
    return allComponents;
}

std::shared_ptr<StaticPolyline> Terrain::getPolyRepresentationOfGround(const floatType resolution) const {
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

void Terrain::sortIntersections(const Line &line, std::vector<std::shared_ptr<Intersection>> &intersections) {
    sortIntersections(line.start, intersections);
}

floatType Terrain::mapHeightToTerrain(const Vector &point) {
    Line intersectionLineUp = {point, {point.x, this->boundingBox.maxMax.y + 5.0f}};
    const std::vector<std::shared_ptr<Intersection>> intersectionsUp = this->getAllIntersections(intersectionLineUp);

    Line intersectionLineDown = {point, {point.x, this->boundingBox.minMin.y - 5.0f}};
    const std::vector<std::shared_ptr<Intersection>> intersectionsDown =
        this->getAllIntersections(intersectionLineDown);

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

Terrain Terrain::getEmptyTerrain(HikerConstants &hikerConstants,
                                 TerrainConstants &terrainConstants,
                                 ResourceManager &resourceManager,
                                 AxisAlignedBoundingBox boundingBox) {
    return {hikerConstants, terrainConstants, resourceManager, boundingBox};
}

Terrain::Terrain(HikerConstants &hikerConstants,
                 TerrainConstants &terrainConstants,
                 ResourceManager &resourceManager,
                 const AxisAlignedBoundingBox boundingBox)
    : hikerConstants(hikerConstants), terrainConstants(terrainConstants), resourceManager(resourceManager) {
    this->boundingBox = boundingBox;
    this->biomes = std::vector<std::shared_ptr<Biome>>{};
}

void Terrain::reset() {
    this->biomes.clear();
    this->boundingBox = {};
    this->generateInitial();
}

std::vector<std::shared_ptr<StaticPolyline>>
Terrain::getTerrainSections(const AxisAlignedBoundingBox &boundingBox) const {
    std::vector<std::shared_ptr<StaticPolyline>> relevantSections = {};
    for (auto const &biome : this->biomes) {
        if (biome->getBoundingBox().overlaps(boundingBox)) {
            auto newGroundSections = biome->getGroundCollisionDetection()->calculateRelevantSections(boundingBox);
            relevantSections.insert(relevantSections.cend(), newGroundSections.cbegin(), newGroundSections.cend());
            for (auto const &component : biome->getComponentsCollisionDetection()) {
                auto newComponentSections = component->calculateRelevantSections(boundingBox);
                relevantSections.insert(relevantSections.cend(), newComponentSections.cbegin(),
                                        newComponentSections.cend());
            }
        }
    }
    return relevantSections;
}

std::vector<std::shared_ptr<Intersection>>
Terrain::calculateCollisionsWithPolygon(const std::shared_ptr<StaticPolygon> &poly) const {
    const AxisAlignedBoundingBox boundingBox = poly->getBoundingBox();
    const auto &mountainSurface = this->getTerrainSections(boundingBox);
    std::vector<std::shared_ptr<Intersection>> intersections = {};
    for (const auto &polyline : mountainSurface) {
        std::vector<std::shared_ptr<Intersection>> newIntersections = polyline->calculateIntersections(poly);
        intersections.insert(intersections.cend(), newIntersections.cbegin(), newIntersections.cend());
    }
    return intersections;
}

std::vector<std::shared_ptr<StaticPolyline>>
Terrain::getTerrainSectionsContinuous(const AxisAlignedBoundingBox &boundingBox) const {
    std::vector<std::shared_ptr<StaticPolyline>> relevantSections = {};
    for (auto const &biome : this->biomes) {
        if (biome->getBoundingBox().overlaps(boundingBox)) {
            auto newGroundSections =
                biome->getGroundCollisionDetection()->calculateRelevantSectionsContinuous(boundingBox);
            relevantSections.insert(relevantSections.cend(), newGroundSections.cbegin(), newGroundSections.cend());
            for (auto const &component : biome->getComponentsCollisionDetection()) {
                auto newComponentSections = component->calculateRelevantSectionsContinuous(boundingBox);
                relevantSections.insert(relevantSections.cend(), newComponentSections.cbegin(),
                                        newComponentSections.cend());
            }
        }
    }
    return relevantSections;
}

floatType Terrain::mapToClosestTopTerrain(const Vector &point) {
    const floatType xPos = point.x;
    Line intersectionLine = {{xPos, this->boundingBox.minMin.y - 5.0f}, {xPos, this->boundingBox.maxMax.y + 5.0f}};
    std::vector<std::shared_ptr<Intersection>> intersections = this->getAllIntersections(intersectionLine);

    sortIntersections(point, intersections);

    bool upwardsAllowed = true;
    bool downwardsAllowed = true;
    int index = 0;
    while (index < intersections.size() && (upwardsAllowed || downwardsAllowed)) {
        auto const &intersection = intersections.at(index).get();
        if (intersection->intersection.y >= point.y && upwardsAllowed) {
            if (intersection->normalAtIntersection.y > 0) { // Top of terrain
                return intersection->intersection.y;
            } else { // Bottom of terrain
                upwardsAllowed = false;
            }
        } else if (intersection->intersection.y < point.y && downwardsAllowed) {
            if (intersection->normalAtIntersection.y > 0) { // Top of terrain
                return intersection->intersection.y;
            } else { // Bottom of terrain
                downwardsAllowed = false;
            }
        }
        index++;
    }

    return point.y; // No terrain height found, leave unchanged
}

void Terrain::sortIntersections(const Vector &point, std::vector<std::shared_ptr<Intersection>> &intersections) {
    std::sort(intersections.begin(), intersections.end(),
              [&](const std::shared_ptr<Intersection> &firstIntersection,
                  const std::shared_ptr<Intersection> &secondIntersection) {
                  // Compare squared distances from the given point
                  return firstIntersection->intersection.distanceTo(point) <
                         secondIntersection->intersection.distanceTo(point);
              });
}

void Terrain::initialize(std::vector<Vector> &groundPoints) {
    this->biomes = {
        std::make_shared<Biome>(Biome(groundPoints, 0.f, hikerConstants, terrainConstants, resourceManager))};
    this->boundingBox = this->biomes.front()->getBoundingBox();
}

void Terrain::initialize() {
    const Vector basePoint{0, 0};
    const Vector baseDerivative{1, 0};
    const floatType startT = 0;
    const auto initialBiome =
        std::make_shared<Biome>(Mountain(basePoint, baseDerivative, startT, this->terrainConstants.biomeWidth,
                                         this->hikerConstants, this->terrainConstants, this->resourceManager, true));
    this->biomes.push_back(initialBiome);
    this->boundingBox = initialBiome->getBoundingBox();
}
