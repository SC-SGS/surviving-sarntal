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
    Terrain(HikerConstants hikerConstants, TerrainConstants terrainConstants);

    /**
     * Generates a biome of the given type on the right side of the currently existing terrain.
     * IMPORTANT: The given length is the length the new biome will have AT LEAST. The biome will generate points until
     * the length is reached or exceeded.
     *
     * @param newBiome
     * @param length rough length of the new terrain to generate.
     */
    void generateBiome(BiomeType newBiome, floatType length);

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
    floatType mapHeightToTerrain(Vector point);

    /**
     * Computes a list of all intersections of the given line between the start and end point with the terrain in the
     * order in which they occur following the line from start to end.
     *
     * @return list of all intersections
     */
    std::vector<Intersection> getAllIntersections(Line &line);

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

    std::vector<StaticPolygon *> getPolyRepresentationOfAllComponentsRendering() const;

    StaticPolyline *getPolyRepresentationOfGroundRendering() const;

    std::vector<StaticPolygon *> getPolyRepresentationOfAllComponentsCollisionDetection() const;

    StaticPolyline *getPolyRepresentationOfGroundCollisionDetection() const;

    StaticPolyline *getBasepoints() const;

    /**
     * Creates an empty terrain with the specified dimensions
     *
     * @return
     */
    static Terrain getEmptyTerrain(HikerConstants hikerConstants, TerrainConstants terrainConstants,
                                   AxisAlignedBoundingBox boundingBox);

  private:
    Terrain(HikerConstants hikerConstants, TerrainConstants terrainConstants, AxisAlignedBoundingBox boundingBox);

    /**
     * Approximates all components of the terrain with polygons of the given resolution and returns a list of all of
     * them.
     *
     * @param resolution
     * @return List of all approximated components
     */
    std::vector<StaticPolygon *> getPolyRepresentationOfAllComponents(floatType resolution) const;

    /**
     * Approximates the ground with a polygon of the given resolution.
     *
     * @param resolution
     * @return
     */
    StaticPolyline *getPolyRepresentationOfGround(floatType resolution) const;

    void recalculateBoundingBox();

    std::vector<Biome *> biomes;
    AxisAlignedBoundingBox boundingBox = {};

    HikerConstants hikerConstants;
    TerrainConstants terrainConstants;

    void generateInitial();

    /**
     * Sorts the given list of intersections by the distance from the start point of the given line.
     *
     * @param line
     */
    static void sortIntersections(Line &line, std::vector<Intersection> &intersections);
};

#endif // SURVIVING_SARNTAL_TERRAIN_HPP
