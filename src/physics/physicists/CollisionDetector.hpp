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
#include "common/shapes.h"

struct PolygonProjectionOnAxis {
    floatType lowerBound{std::numeric_limits<floatType>::infinity()};
    floatType upperBound{-std::numeric_limits<floatType>::infinity()};
    size_t indexOfLowestVertex{0};

    bool overlaps(const PolygonProjectionOnAxis &other, const floatType eps = NUMERIC_EPSILON) const {
        return lowerBound + eps < other.upperBound && upperBound > other.lowerBound + eps;
    }
};

struct CollisionObject {
    bool isCollision{false};
    floatType collisionDepth{std::numeric_limits<floatType>::max()};
    Vector collisionDirection{};
};

struct PolygonCollisionObject : CollisionObject {
    // Body containing the vertex
    ConvexPolygon *polyAIncident{nullptr};
    // Body containing the face having the collision normal, which points from B to A
    ConvexPolygon *polyBReference{nullptr};
    // Colliding vertex on polyA
    size_t collisionVertexIdx{};
    // Index of the reference edge on polyB
    size_t collisionFaceIdx{};
    // Index of the reference edge on polyB if no collision occurred
    size_t lastWitnessEdgeIdx{};
    // TODO Collision Manifold
};

struct DynamicPolygonCollisionObject : CollisionObject {
    // Body containing the vertex
    DynamicConvexPolygon *polyAIncident{nullptr};
    // Body containing the face having the collision normal, which points from B to A
    DynamicConvexPolygon *polyBReference{nullptr};
    // Colliding vertex on polyA
    size_t collisionVertexIdx{};
    // Index of the reference edge on polyB
    size_t collisionFaceIdx{};
    // TODO colllision manifold
    // alpha-value between 0 and 1 at which collision first occurred
    floatType relativeTimeOfCollision{1};
};

struct DynamicPolygonTerrainCollisionObject : CollisionObject {
    DynamicConvexPolygon *poly{nullptr};
    std::optional<SimpleConvexPolygon> triangle{};
    bool isPolyIncident{true};
    // Colliding vertex on incident polygon
    size_t collisionVertexIdx{};
    // Index of the reference edge
    size_t collisionFaceIdx{};
    // alpha-value between 0 and 1 at which collision first occurred
    floatType relativeTimeOfCollision{1};
};

struct LineAABBIntersection {
    enum Type { LEFT, RIGHT, TOP, BOTTOM, NO_INTERSECTION };
    Type intersectionType{NO_INTERSECTION};
    std::optional<Vector> intersectionPoint;
};

/**
 * TODO this does not work, but I guess we can assume that the collision handler only calls this when knowing the
*participating rocks...
struct PolygonCollisionObject : CollisionObject { std::shared_ptr<DynamicPolygon> poly1{};
    std::shared_ptr<DynamicPolygon> poly2{};
};

struct TerrainCollisionObject : CollisionObject {
    const std::shared_ptr<DynamicPolygon> poly{};
};*/

// TODO so far this is a collection of evil utils, pls come pick me up, I'm scared
class CollisionDetector {

  public:
    explicit CollisionDetector(World &world, const GameConstants &gameConstants, bool devMode = false);
    ~CollisionDetector() = default;

    /**
     * TODO This should later return a collision object with a list of collided rock IDs, player is hit and damage to
     * TODO player or be united with the handler
     * TODO or a list of collision objects or something similar
     */
    void detectCollisions() const;

    // TODO linked cell?

    /**
     * Checks whether two given circular rocks collide.
     *
     * @param rock1
     * @param rock2
     * @return
     *
    static bool rocksCollide(Rock &rock1, Rock &rock2);*/

    /**
     * Checks whether the player is hit by a given rock, where the player is represented as a axis-aligned rectangle and
     * the rock as a circle.
     * See
     * <a href="https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection">this</a>.
     *
     * @return
     *
    bool isPlayerHitByRock(Rock &rock);*/

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
    static ConcavePolygon
    getConcaveTerrainPolygonInAABB(const std::vector<std::shared_ptr<StaticPolyline>> &terrainSurfaceComponents,
                                   const AABB &aabb);

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
};

#endif // COLLISIONDETECTOR_H
