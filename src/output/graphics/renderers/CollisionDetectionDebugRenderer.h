//
// Created by bjoern on 9/28/24.
//

#ifndef COLLISIONDETECTIONDEBUGRENDERER_H
#define COLLISIONDETECTIONDEBUGRENDERER_H

#include "../../../entities/World.h"
#include "../../../utilities/CollisionUtils.h"

class CollisionDetectionDebugRenderer {
  public:
    CollisionDetectionDebugRenderer(const World &world, const GameConstants &gameConstants, Camera2D &camera);

    void debugRenderRockCollision(const DynamicPolygonCollisionObject &result);

    /**
     * Does what it says. Careful, only call when result is actually a collision, i.e. the optional triangle has a
     * value.
     *
     * @param result
     * @param triangles
     * @param aabb
     */
    void debugTerrainCollisionRendering(const DynamicPolygonTerrainCollisionObject &result,
                                        const std::vector<SimpleConvexPolygon> &triangles,
                                        const AABB &aabb);

  private:
    const World &world;
    const GameConstants &gameConstants;
    Camera2D &camera;

    static void debugRenderPolygonCollisionEdgeVertexNormal(const DynamicPolygonCollisionObject &result);

    static void debugRenderTriangulatedTerrain(const std::vector<SimpleConvexPolygon> &triangles);

    static void debugRenderAABB(const AABB &aabb);

    static void debugRenderCollidingTriangle(const SimpleConvexPolygon &triangle);

    static void debugRenderTerrainCollisionEdgeVertexAndNormal(const DynamicPolygonTerrainCollisionObject &result);
};

#endif // COLLISIONDETECTIONDEBUGRENDERER_H
