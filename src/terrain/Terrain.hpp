//
// Created by felix-roehr on 7/25/24.
//

#ifndef SURVIVING_SARNTAL_TERRAIN_HPP
#define SURVIVING_SARNTAL_TERRAIN_HPP

#include "../game/GameProperties.hpp"
#include "biomes/Biome.hpp"
#include "biomes/BiomeType.hpp"
#include "components/Background.hpp"
#include "components/ClosedComponent.hpp"
#include "components/Ground.hpp"
#include "components/Splineoid.hpp"

class Terrain {
  public:
    Terrain(HikerConstants &hikerConstants, TerrainConstants &terrainConstants, ResourceManager &resourceManager);

    /**
     * Generates a biome of the given type on the right side of the currently existing terrain.
     * IMPORTANT: The given length is the length the new biome will have AT LEAST. The biome will generate points until
     * the length is reached or exceeded.
     *
     * @param biomeType
     * @param length rough length of the new terrain to generate.
     */
    std::shared_ptr<Biome> generateBiome(BiomeType biomeType, floatType length) const;

    /**
     * Adds the given biome to this terrain.
     * IMPORTANT: The start of the given biome must fit to the end of the existing terrain.
     *
     * @param biome
     */
    void addBiome(const std::shared_ptr<Biome> &biome);

    /**
     * Removes all complete biomes left of the specified xPos.
     * @param xPos
     */
    void removeBiome(float xPos);

    /**
     * Returns the height of the ground (lowest) terrain at the given xPos
     *
     * @param xPos
     */
    floatType getGroundHeight(floatType xPos);

    /**
     * Returns the height of the highest terrain component at the given xPos
     *
     * @param xPos
     */
    floatType getMaxHeight(floatType xPos);

    /**
     * Calculates the height of the terrain closest to the given point.
     *
     * @param point
     * @return
     */
    floatType mapHeightToTerrain(const Vector &point);

    /**
     * Maps the given position to the closest terrain height that is an upper side of the terrain with no underside of
     * the terrain in between.
     *
     * @param point Position to map
     * @return
     */
    floatType mapToClosestTopTerrain(const Vector &point);

    /**
     * Computes a list of all intersections of the given line between the start and end point with the terrain in the
     * order in which they occur following the line from start to end.
     *
     * @return list of all intersections
     */
    std::vector<std::shared_ptr<Intersection>> getAllIntersections(Line &line);

    /**
     * Computes the xPos at the end of the terrain.
     * @return
     */
    floatType getRightBorder() const;

    /**
     * Computes the xPos at the start of the terrain.
     * @return
     */
    floatType getLeftBorder() const;

    /**
     * Computes the maximum height of the terrain.
     * @return
     */
    floatType getMaxHeight() const;

    /**
     * Computes the minimum height of the terrain.
     * @return
     */
    floatType getMinHeight() const;

    std::vector<std::shared_ptr<StaticPolygon>> getPolyRepresentationOfAllComponentsRendering() const;

    std::vector<std::shared_ptr<StaticPolyline>> getPolyRepresentationOfGroundRendering() const;

    /**
     * Returns all sections of this terrain that overlap the given bounding box
     *
     * @param boundingBox
     * @return relevant terrain sections as polylines
     */
    std::vector<std::shared_ptr<StaticPolyline>> getTerrainSections(const AxisAlignedBoundingBox &boundingBox) const;

    /**
     * Returns all sections of this terrain that overlap the given bounding box in the order given by the terrain and
     * without gaps.
     *
     * @param boundingBox
     * @return relevant terrain sections as polylines
     */
    std::vector<std::shared_ptr<StaticPolyline>>
    getTerrainSectionsContinuous(const AxisAlignedBoundingBox &boundingBox) const;

    std::shared_ptr<StaticPolyline> getBasepoints() const;

    /**
     * Creates an empty terrain with the specified dimensions
     *
     * @return
     */
    static Terrain getEmptyTerrain(HikerConstants &hikerConstants,
                                   TerrainConstants &terrainConstants,
                                   ResourceManager &resourceManager,
                                   AxisAlignedBoundingBox boundingBox);

    void reset();

    /**
     * Chacks whether the given polygon collides with the terrain.
     *
     * @param poly
     * @return
     */
    std::vector<std::shared_ptr<Intersection>>
    calculateCollisionsWithPolygon(const std::shared_ptr<StaticPolygon> &poly) const;

    /**
     * Sorts the given list of intersections by the distance from the start point of the given line.
     *
     * @param line
     */
    static void sortIntersections(const Line &line, std::vector<std::shared_ptr<Intersection>> &intersections);

    /**
     * Sorts the given list of intersections by the distance from the given point.
     *
     * @param line
     */
    static void sortIntersections(const Vector &point, std::vector<std::shared_ptr<Intersection>> &intersections);

    /**
     * Generates the terrain randomly
     */
    void initialize();

    /**
     * Generates the terrain with the given ground points;
     */
    void initialize(std::vector<Vector> &groundPoints);

  private:
    std::vector<std::shared_ptr<Biome>> biomes;
    AxisAlignedBoundingBox boundingBox = {};

    HikerConstants &hikerConstants;
    TerrainConstants &terrainConstants;
    ResourceManager &resourceManager;

    Terrain(HikerConstants &hikerConstants,
            TerrainConstants &terrainConstants,
            ResourceManager &resourceManager,
            AxisAlignedBoundingBox boundingBox);

    /**
     * Approximates all components of the terrain with polygons of the given resolution and returns a list of all of
     * them.
     *
     * @param resolution
     * @return List of all approximated components
     */
    std::vector<std::shared_ptr<StaticPolygon>> getPolyRepresentationOfAllComponents(floatType resolution) const;

    /**
     * Approximates the ground with a polygon of the given resolution.
     *
     * @param resolution
     * @return
     */
    std::shared_ptr<StaticPolyline> getPolyRepresentationOfGround(floatType resolution) const;

    void recalculateBoundingBox();

    void generateInitial();
};

#endif // SURVIVING_SARNTAL_TERRAIN_HPP
