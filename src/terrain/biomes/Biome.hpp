//
// Created by felix-roehr on 7/25/24.
//

#ifndef SURVIVING_SARNTAL_BIOME_HPP
#define SURVIVING_SARNTAL_BIOME_HPP

#include "../components/Background.hpp"
#include "../components/Ground.hpp"
#include "../components/structures/Structure.hpp"
#include "../points/StaticPolyObject.hpp"
#include "../spatialDatastructure/AxisAlignedBoundingBox.hpp"
#include "TerrainPhase.hpp"
#include "raylib.h"

class Biome {
  public:
    Biome(const std::vector<TerrainPhase> &phaseChoices, Vector basePoint, Vector baseDerivative, floatType startT,
          floatType length, HikerConstants hikerConstants, TerrainConstants terrainConstants);
    virtual ~Biome() = default;

    /**
     * Generates derivatives for the given points. Here, the derivatives are generated as linear interpolation between
     * the adjacent points.
     *
     * @param points
     * @return
     */
    static std::vector<Vector> generateSmoothDerivatives(std::vector<Vector> points);

    /**
     * Generates derivatives for the given points. Here, the derivatives are first generated as linear interpolation
     * between the adjacent points, then a random rotational offset is applied to them according to the given
     * randomness.
     *
     * @param points
     * @param randomness
     * @return
     */
    static std::vector<Vector> generateSemiSmoothDerivatives(std::vector<Vector> points, floatType randomness);

    Ground *getGround();

    floatType getLeftBorder();

    floatType getRightBorder();

    /**
     * Computes a list of all intersections of the given line between the start and end point with the terrain in the
     * order in which they occur following the line from start to end.
     *
     * @return list of all intersections
     */
    std::vector<Intersection> getAllIntersections(Line &line);

    /**
     * Approximates all components of the biome with polygons of the given resolution and returns a list of all of them.
     *
     * @param resolution
     * @return List of all approximated components
     */
    std::vector<StaticPolygon *> calculatePolyRepresentationOfAllComponents(floatType resolution) const;

    /**
     * Approximates the ground with a polygon of the given resolution.
     *
     * @param resolution
     * @return
     */
    StaticPolyline *calculatePolyRepresentationOfGround(floatType resolution) const;

    /**
     * Updates the saved polyrepresentations for rendering and collision detection.
     */
    void updatePolyRepresentations();

    StaticPolyline *getGroundRendering() const;
    const std::vector<StaticPolygon *> &getComponentsRendering() const;
    StaticPolyline *getGroundCollisionDetection() const;
    const std::vector<StaticPolygon *> &getComponentsCollisionDetection() const;

    const AxisAlignedBoundingBox &getBoundingBox() const;

    static Vector2 transformPosition(Vector position);

  protected:
    const int generatedPoints = 150;
    const std::vector<TerrainPhase> phaseChoices;

    const floatType leftBorder;
    floatType rightBorder;

    std::vector<ClosedComponent *> components;
    Ground *ground;

    AxisAlignedBoundingBox boundingBox{};

    /**
     * Tries to place the given Structure within the given space limits. If the constraints for the structure are
     * fulfilled in this range, the structure is placed.
     *
     * @return true, if the structure has been placed successfully, false otherwise
     */
    virtual bool placeStructure(AxisAlignedBoundingBox spacelimit, Structure *structure);

    /**
     * Places structures where possible and according to their spawn probability.
     */
    virtual void placeStructures();

  private:
    HikerConstants hikerConstants;
    TerrainConstants terrainConstants;

    StaticPolyline *groundRendering = {};
    std::vector<StaticPolygon *> componentsRendering = {};

    StaticPolyline *groundCollisionDetection = {};
    std::vector<StaticPolygon *> componentsCollisionDetection = {};

    /**
     * Returns a random phase from the phases of this biome
     *
     * @return the random phase
     */
    TerrainPhase getRandomPhase() const;

    /**
     * Generates a random new point that matches the specified terrain phase based on the given point.
     *
     * @param phase
     * @param basePoint
     * @return
     */
    Vector generateNextPoint(TerrainPhase phase, Vector basePoint) const;

    /**
     * Generates a random new point that matches the specified terrain phase based on the given point.
     * Here, we rotate the average direction by a random angle in the range [minAngle, maxAngle] and add the result to
     * the given basePoint.
     *
     * @param basePoint
     * @param minAngle
     * @param maxAngle
     * @return
     */
    Vector generateNextPoint(TerrainPhase phase, Vector basePoint, floatType minAngle, floatType maxAngle) const;

    /**
     * Generates a random new point that matches the specified terrain phase and takes the nSmoothness past points into
     * consideration as well.
     *
     * @param phase
     * @param oldPoints
     * @param nSmoothness
     * @return
     */
    Vector generateNextPointContinuous(TerrainPhase phase, std::vector<Vector> oldPoints, int nSmoothness) const;

    /**
     * Generates the initial ground containing only two points;
     *
     * @param basePoint
     * @param baseDerivative
     * @param startT
     * @param phase initial generation phase
     */
    Ground *generateGroundBase(Vector basePoint, Vector baseDerivative, floatType startT, TerrainPhase phase) const;

    /**
     * Generates the ground starting from the given basePoint.
     *
     * @param basePoint
     * @param baseDerivative
     * @param phase initial generation phase
     */
    Ground *generateGround(Vector basePoint, Vector baseDerivative, floatType startT) const;

    /**
     * Checks whether the given new point is a viable continuation of the given oldPoints.
     *
     * @param newPoint
     * @param oldPoints
     * @return true, if all constraints are fulfilled
     */
    bool fulfillsConstraints(Vector newPoint, Vector newDerivative, const Ground *newGround,
                             const StaticPolyline *currentPolyRepresentation) const;

    bool fulfillsHikerClearance(const StaticPolyline *currentPolyRepresentation,
                                const StaticPolyline *additionalTerrain, Vector newPoint) const;

    bool fulfillsNoSelfPenetration(const StaticPolyline *currentPolyRepresentation,
                                   StaticPolyline *additionalTerrain) const;

    bool fulfillsMinimalBasePointDistance(const StaticPolyline *currentPolyRepresentation,
                                          const StaticPolyline *additionalTerrain, Vector newPoint) const;

    bool fulfillsMinimalBasePolylineAngle(const StaticPolyline *currentBasePoints, Vector newPoint) const;

    bool fulfillsMaximumOverhangDepth(const StaticPolyline *currentPolyRepresentation,
                                      StaticPolyline *additionalTerrain) const;

    bool fulfillsOverhangJumpable(const StaticPolyline *currentPolyRepresentation,
                                  StaticPolyline *additionalTerrain) const;

    /**
     * Calculates the minimal angle such that a point generated using this angle fulfills all constraints.
     * The new point is generated using the last point of oldPoints as the basis point and the angle is applied to the
     * average direction in the given terrain phase.
     *
     * The method uses an iterative process to find this minimum.
     *
     * @param phase
     * @param newGround
     * @return
     */
    std::optional<floatType> getMinGenerationAngle(TerrainPhase phase, Ground *newGround,
                                                   const StaticPolyline *groundPolyRepresentation) const;

    /**
     * Calculates the maximal angle such that a point generated using this angle fulfills all constraints.
     * The new point is generated using the last point of oldPoints as the basis point and the angle is applied to the
     * average direction in the given terrain phase.
     *
     * The method uses an iterative process to find this maximum.
     *
     * @param phase
     * @param newGround
     * @return
     */
    std::optional<floatType> getMaxGenerationAngle(TerrainPhase phase, Ground *newGround,
                                                   const StaticPolyline *groundPolyRepresentation) const;

    Camera2D setUpGenerationRendering() const;

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
    bool isOutsideHikerClearance(Vector oldPoint, const StaticPolyline *newTerrain, Vector direction) const;

    /**
     * Checks whether the new point is outside the range of the hiker clearance in any direction from the old point.
     *
     * @param newPoint
     * @param oldPoint
     * @return
     */
    bool isOutsideHikerClearanceAnyDirection(Vector oldPoint, Vector newPoint) const;

    /**
     * Renders the mountain during generation;
     */
    void generationRendering(Ground *newGround, const StaticPolyline *groundPolyRepresentation, Vector oldPoint,
                             Vector newPoint, TerrainPhase phase, floatType minAngle, floatType maxAngle) const;

    /**
     * Computes the bounding box of this terrain based on the bounding boxes of all components and the ground.
     */
    void computeBoundingBox();
};

#endif // SURVIVING_SARNTAL_BIOME_HPP
