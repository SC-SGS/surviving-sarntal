//
// Created by bjoern on 5/21/24.
//

#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "../../entities/World.h"
#include "../../geometry/AABB.h"
#include "../../geometry/ConcavePolygon.h"
#include "../../geometry/ConvexPolygon.h"
#include "../../geometry/DynamicConvexPolygon.h"
#include "../../geometry/SimpleConvexPolygon.h"
#include "../../output/graphics/renderers/CollisionDetectionDebugRenderer.h"
#include "../../utilities/CollisionUtils.h"
#include "common/shapes.h"

class CollisionDetector {

  public:
    explicit CollisionDetector(World &world,
                               const GameConstants &gameConstants,
                               CollisionDetectionDebugRenderer &collisionRenderer,
                               bool devMode = false);
    ~CollisionDetector() = default;

    /**
     * Checks whether the player is hit by a given rock, where the player is represented by their hitbox.
     *
     * @return
     */
    DynamicPolygonCollisionObject playerRockCollision(Rock &rock) const;

    /**
     * Checks whether two convex dynamic polygons have collided in the last time step of size deltaT.
     * Only not const params because pointers to the colliding polygons are returned in the collision object.
     *
     * @param poly1
     * @param poly2
     * @return
     */
    DynamicPolygonCollisionObject dynamicPolygonCollision(DynamicConvexPolygon &poly1,
                                                          DynamicConvexPolygon &poly2) const;

    /**
     * Returns a list of collisions of a polygon with the terrain. The terrain is approximated by a continuous line.
     *
     * @param poly
     * @return
     */
    DynamicPolygonTerrainCollisionObject polygonTerrainCollision(DynamicConvexPolygon &poly) const;

    /**
     * Checks whether two polygons collide using the SAT:
     *
     * for each normal of edge of poly 1
     *  project all vertices of both polygons on normal and check for overlap
     * for each normal of edge of poly 2
     *  project all vertices of both polygons on normal and check for overlap
     * return isOverlap
     *
     *
     * @param poly1
     * @param poly2
     * @param poly1LastWitnessToPoly2
     * @param poly2LastWitnessToPoly1
     * @param eps
     * @return
     */
    static PolygonCollisionObject polygonsCollide(ConvexPolygon &poly1,
                                                  ConvexPolygon &poly2,
                                                  size_t poly1LastWitnessToPoly2 = 0,
                                                  size_t poly2LastWitnessToPoly1 = 0,
                                                  floatType eps = NUMERIC_EPSILON);

  private:
    World &world;
    const GameConstants &gameConstants;
    const bool devMode;
    CollisionDetectionDebugRenderer &collRenderer;

    /**
     * Checks if there is a possible collision of two dynamic convex polygons in the last time step by checking whether
     * their swept bounding boxes collide.
     *
     * @param poly1
     * @param poly2
     * @return
     */
    static bool sweptAABBsCollide(const DynamicConvexPolygon &poly1, const DynamicConvexPolygon &poly2);

    /**
     * Checks whether the separating axis of the two polygons is one of the edges of poly 1.
     *
     * @param poly1
     * @param poly2
     * @param poly1LastWitnessToPoly2
     * @param eps
     * @return the collision with the smallest depth
     */
    static PolygonCollisionObject collisionWithSepAxisOn1(ConvexPolygon &poly1,
                                                          ConvexPolygon &poly2,
                                                          size_t poly1LastWitnessToPoly2 = 0,
                                                          floatType eps = NUMERIC_EPSILON);

    /**
     *
     * @param result
     * @param proj1
     * @param proj2
     * @param vertices1
     * @param normal
     * @param collisionFaceIdx
     */
    static void updateCollisionResultAfterOverlapFound(PolygonCollisionObject &result,
                                                       const PolygonProjectionOnAxis &proj1,
                                                       const PolygonProjectionOnAxis &proj2,
                                                       const std::vector<Vector> &vertices1,
                                                       const Vector &normal,
                                                       size_t collisionFaceIdx);

    /**
     * Projects a Polygon on a Normal vector, returning a 1D PolygonProjection represented by its lower and upper bound
     * on the axis..
     *
     * @param poly a convex 2D polygon
     * @param normal of the other polygon, must be normalized
     * @return
     */
    static PolygonProjectionOnAxis projectPolygonOnNormal(const ConvexPolygon &poly, const Vector &normal);

    /**
     * Creates a dynamic polygon collision object from a polygon collision object and additional temporal information.
     *
     * @param collResult
     * @param poly1
     * @param poly2
     * @param poly1AtT
     * @param poly2AtT
     * @param relativeTime
     * @return
     */
    static DynamicPolygonCollisionObject
    createDynamicPolygonCollisionObjectFrom(const PolygonCollisionObject &collResult,
                                            DynamicConvexPolygon &poly1,
                                            DynamicConvexPolygon &poly2,
                                            const ConvexPolygon &poly1AtT,
                                            const ConvexPolygon &poly2AtT,
                                            floatType relativeTime);

    /**
     * Uses according method from terrain and puts the components together to one polsline.
     *
     * @param terrainSurfaceComponents
     * @return
     */
    static StaticPolyline getContinuousTerrainSurfaceFromComponents(
        const std::vector<std::shared_ptr<StaticPolyline>> &terrainSurfaceComponents);

    /**
     * Returns a concave polygon representing the terrain in a given bounding box.
     *
     * @param terrainSurfaceComponents
     * @param aabb
     * @return
     */
    static std::optional<ConcavePolygon>
    getConcaveTerrainPolygonInAABB(const std::vector<std::shared_ptr<StaticPolyline>> &terrainSurfaceComponents,
                                   const AABB &aabb);

    /**
     * Returns a concave polygon representing the terrain in a given bounding box.
     *
     * @param terrainSurfacePoints list of relevant terrain points. Must be inside the completeAABB
     * @param completeAABB aabb that encloses all the terrainSurfacePoints and th searchAABB
     * @param entryPoint point on the completeAABB, that is used as a reference for where the terrain starts.
     * @param exitPoint point on the completeAABB, that is used as a reference for where the terrain ends.
     * @param indexExitLine index of the line of the completeAABB, on which the exitPoint is.
     * @return
     */
    static std::optional<ConcavePolygon> getConcaveTerrainPolygonInAABB(const std::vector<Vector> &terrainSurfacePoints,
                                                                        const AxisAlignedBoundingBox &completeAABB,
                                                                        const Vector &entryPoint,
                                                                        const Vector &exitPoint,
                                                                        int indexExitLine);

    /**
     * Returns the boundary triangles of the triangulation of the concave polygon representing the terrain.
     *
     * @param terrainPoly
     * @return
     */
    static std::vector<SimpleConvexPolygon> getSurfaceTrianglesOfConcaveTerrainPoly(const ConcavePolygon &terrainPoly);

    /**
     * Triangulates the given concave polygon representing the terrain using the poly2tri library.
     *
     * @param terrainPoly
     * @return
     */
    static std::vector<SimpleConvexPolygon> triagulateConcaveTerrainPoly(const ConcavePolygon &terrainPoly);

    /**
     * Returns a SimpleConvexPolygon representation of a p2t triangle.
     *
     * @param p2tTri
     * @return
     */
    static SimpleConvexPolygon createConvexPolyFromP2TTriangle(p2t::Triangle &p2tTri);

    /**
     * Returns a DynamicPolygonTerrainCollisionObject from a PolygonCollisionObject and some additional info.
     *
     * @param collision
     * @param poly
     * @param polyAtT
     * @param triangle
     * @param relativeTime
     * @return
     */
    static DynamicPolygonTerrainCollisionObject
    createDynamicPolygonTerrainCollisionObjectFrom(const PolygonCollisionObject &collision,
                                                   DynamicConvexPolygon &poly,
                                                   const SimpleConvexPolygon &polyAtT,
                                                   const ConvexPolygon &triangle,
                                                   floatType relativeTime);

    /**
     * Removes lines from the start and end of this polyline that do not intersect the aabb.
     *
     * @param terrainSection
     * @param searchAABB
     * @return
     */
    static std::optional<StaticPolyline> removeRedundantPoints(const StaticPolyline &terrainSection,
                                                               const AxisAlignedBoundingBox &searchAABB);
};

#endif // COLLISIONDETECTOR_H
