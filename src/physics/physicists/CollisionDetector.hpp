//
// Created by bjoern on 5/21/24.
//

#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "../../entities/World.h"

struct PolygonProjectionOnAxis {
    floatType lowerBound{std::numeric_limits<floatType>::max()};
    floatType upperBound{std::numeric_limits<floatType>::min()};

    bool overlaps(const PolygonProjectionOnAxis &other) const {
        return lowerBound < other.upperBound && upperBound > other.lowerBound;
    }
};

struct PolygonEdge {
    Vector vertexA;
    Vector vertexB;
};

struct CollisionObject {
    bool isCollision{false};
    floatType collisionDepth{};
    Vector collisionDirection{};
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
    explicit CollisionDetector(World &world);
    ~CollisionDetector() = default;

    /**
     * TODO This should later return a collision object with a list of collided rock IDs, player is hit and damage to
     * TODO player or be united with the handler
     * TODO or a list of collision objects or something similar
     */
    void detectCollisions() const;

    // TODO later one collides method that can calculate whether arbritrary objects collide? Smaller ball of mud...
    // TODO this method should then also be tested well

    /**
     * Checks whether two given circular rocks collide.
     *
     * TODO maybe later adapt to non-circular rocks and it should be used in some method that uses linked cell or sth
     *
     * @param rock1
     * @param rock2
     * @return
     */
    static bool rocksCollide(Rock &rock1, Rock &rock2);

    /**
     * Checks whether the player is hit by a given rock, where the player is represented as a axis-aligned rectangle and
     * the rock as a circle.
     * See
     * <a href="https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection">this</a>.
     *
     * TODO there should be one method to detect all collisions and return a collObj this method should be tested well
     *
     * @return
     */
    bool isPlayerHitByRock(Rock &rock);

    /**
     * Used to calculate the normal... AT A VERTEX!!!
     * Only useful because we still have this terrible implementation, where the rocks don't collide with the lines of
     * the mountain, but at the vertices, which are NON-DIFFERENTIABLE  AGHHH.
     * Like... look at this documentation from legacy:
     * Find the normal vector of a given vertex
     * Calculate, which of the neighbouring points is closer and compute the
     * normal corresponding to the connection line between these two
     *
     * TODO this is evil and has to go
     *
     * @param idx
     * @param rockPos
     * @return
     */
    Vector getNormal(Vector rockPos) const;

    /**
     * Returns a list of collisions of a polygon with the terrain. The terrain is approximated by a continuous line.
     *
     * @param poly
     * @param terrain
     * @return
     */
    static std::vector<CollisionObject> polygonTerrainCollision(const DynamicPolygon &poly, const Terrain &terrain);

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
     * @return
     */
    static CollisionObject polygonsCollide(const DynamicPolygon &poly1, const DynamicPolygon &poly2);

  private:
    World &world;

    /**
     * Checks whether a polygon collides with a line that is a boundary to the terrain, i.e. the polygon being on the
     * "inner side" of the line also counts as collision.
     *
     * @param poly
     * @param edge
     * @return
     */
    static CollisionObject polygonLineCollision(const DynamicPolygon &poly, const PolygonEdge &edge);

    /**
     * Checks whether the separating axis of the two polygons is one of the edges of poly 1.
     *
     * @param poly1
     * @param poly2
     * @return the collision with smallest depth
     */
    static CollisionObject collisionWithSepAxisOn1(const DynamicPolygon &poly1, const DynamicPolygon &poly2);

    /**
     * Projects a Polygon on a Normal vector, returning a 1D PolygonProjection represented by its lower and upper bound
     * on the axis..
     *
     * @param poly a convex 2D polygon
     * @param normal of the other polygon, must be normalized
     * @return
     */
    static PolygonProjectionOnAxis projectPolygonOnNormal(const DynamicPolygon &poly, const Vector &normal);
};

#endif // COLLISIONDETECTOR_H
