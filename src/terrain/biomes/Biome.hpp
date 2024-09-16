//
// Created by felix-roehr on 7/25/24.
//

#ifndef SURVIVING_SARNTAL_BIOME_HPP
#define SURVIVING_SARNTAL_BIOME_HPP

#include "../collisionDetection/CollisionDetectionRepresentation.hpp"
#include "../components/Background.hpp"
#include "../components/Ground.hpp"
#include "../components/structures/Structure.hpp"
#include "../points/StaticPolyObject.hpp"
#include "../spatialDatastructure/AxisAlignedBoundingBox.hpp"
#include "TerrainPhase.hpp"
#include "raylib.h"

class Biome {
  public:
    Biome(const std::vector<TerrainPhase> &phaseChoices, const TerrainPhase &firstPhase, Vector &basePoint,
          Vector &baseDerivative, floatType startT, floatType length, HikerConstants &hikerConstants,
          TerrainConstants &terrainConstants, bool renderGeneration);
    virtual ~Biome() = default;

    std::shared_ptr<Ground> getGround();

    floatType getLeftBorder();

    floatType getRightBorder();

    /**
     * Returns a random phase from the phases of this biome
     *
     * @return the random phase
     */
    TerrainPhase getRandomPhase() const;

    /**
     * Computes a list of all intersections of the given line between the start and end point with the terrain in the
     * order in which they occur following the line from start to end.
     *
     * @return list of all intersections
     */
    std::vector<std::shared_ptr<Intersection>> getAllIntersections(Line &line);

    /**
     * Approximates all components of the biome with polygons of the given resolution and returns a list of
     * corresponding collision detection representations.
     *
     * @param resolution
     * @return List of all approximated components
     */
    std::vector<std::shared_ptr<CollisionDetectionRepresentation>>
    calculateCollisionDetectionRepresentationComponents(floatType resolution) const;

    /**
     * Approximates the ground with a polygon of the given resolution and return corresponding a collision detection
     * representation.
     *
     * @param resolution
     * @return
     */
    std::shared_ptr<CollisionDetectionRepresentation>
    calculateCollisionDetectionRepresentationGround(floatType resolution) const;

    /**
     * Approximates all components of the biome with polygons of the given resolution and returns a list of all of them.
     *
     * @param resolution
     * @return List of all approximated components
     */
    std::vector<std::shared_ptr<StaticPolygon>> calculatePolyRepresentationOfAllComponents(floatType resolution) const;

    /**
     * Approximates the ground with a polygon of the given resolution.
     *
     * @param resolution
     * @return
     */
    std::shared_ptr<StaticPolyline> calculatePolyRepresentationOfGround(floatType resolution) const;

    /**
     * Updates the saved polyrepresentations for rendering and collision detection.
     */
    void updatePolyRepresentations();

    const std::shared_ptr<StaticPolyline> &getGroundRendering() const;
    const std::vector<std::shared_ptr<StaticPolygon>> &getComponentsRendering() const;
    const std::shared_ptr<CollisionDetectionRepresentation> &getGroundCollisionDetection() const;
    const std::vector<std::shared_ptr<CollisionDetectionRepresentation>> &getComponentsCollisionDetection() const;

    const AxisAlignedBoundingBox &getBoundingBox() const;

    const TerrainPhase &getFirstPhase() const;

  protected:
    const std::vector<TerrainPhase> phaseChoices;
    const TerrainPhase firstPhase;

    const floatType leftBorder;
    floatType rightBorder;

    std::vector<std::shared_ptr<ClosedComponent>> components;
    std::shared_ptr<Ground> ground;

    AxisAlignedBoundingBox boundingBox{};

  private:
    HikerConstants hikerConstants;
    TerrainConstants terrainConstants;

    std::shared_ptr<StaticPolyline> groundRendering = {};
    std::vector<std::shared_ptr<StaticPolygon>> componentsRendering = {};

    std::shared_ptr<CollisionDetectionRepresentation> groundCollisionDetection = {};
    std::vector<std::shared_ptr<CollisionDetectionRepresentation>> componentsCollisionDetection = {};

    /**
     * Computes the bounding box of this terrain based on the bounding boxes of all components and the ground.
     */
    void computeBoundingBox();
};

#endif // SURVIVING_SARNTAL_BIOME_HPP
