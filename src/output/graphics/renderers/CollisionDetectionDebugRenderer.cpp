//
// Created by bjoern on 9/28/24.
//

#include "CollisionDetectionDebugRenderer.h"
#include "../GraphicsUtil.h"
#include "PolygonRenderer.h"

void CollisionDetectionDebugRenderer::debugRenderRockCollision(const DynamicPolygonCollisionObject &result) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        PolygonRenderer::renderPolygonOutlineStatic(*result.polyAIncident);
        PolygonRenderer::renderPolygonOutlineStatic(*result.polyBReference);
        debugRenderPolygonCollisionEdgeVertexNormal(result);
        EndDrawing();
    }
}

void CollisionDetectionDebugRenderer::debugRenderPolygonCollisionEdgeVertexNormal(
    const DynamicPolygonCollisionObject &result) {
    const Vector vertexPos = GraphicsUtil::transformPosition(
        Vector2(result.polyAIncident->getWorldSpaceVertices()[result.collisionVertexIdx]));
    DrawCircle(static_cast<int>(vertexPos.x), static_cast<int>(vertexPos.y), 7.0f, BLUE);

    const Vector edgeStart = GraphicsUtil::transformPosition(
        Vector2(result.polyBReference->getWorldSpaceVertices()[result.collisionFaceIdx]));
    const Vector edgeEnd = GraphicsUtil::transformPosition(
        Vector2(result.polyBReference->getWorldSpaceVertices()[(result.collisionFaceIdx + 1) %
                                                               result.polyBReference->getWorldSpaceVertices().size()]));
    DrawLineEx(Vector2(edgeStart), Vector2(edgeEnd), 5.0, GREEN);

    const Vector normalStart = vertexPos;
    const Vector normalEnd = GraphicsUtil::transformPosition(
        Vector2(result.polyAIncident->getWorldSpaceVertices()[result.collisionVertexIdx] +
                (result.collisionDirection))); // * result.collisionDepth)));
    DrawLineEx(Vector2(normalStart), Vector2(normalEnd), 5.0, RED);
}

void CollisionDetectionDebugRenderer::debugTerrainCollisionRendering(const DynamicPolygonTerrainCollisionObject &result,
                                                                     const std::vector<SimpleConvexPolygon> &triangles,
                                                                     const AABB &aabb) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        debugRenderTriangulatedTerrain(triangles);
        debugRenderAABB(aabb);
        PolygonRenderer::renderPolygonOutlineStatic(*result.poly);
        debugRenderCollidingTriangle(result.triangle.value());
        debugRenderTerrainCollisionEdgeVertexAndNormal(result);
        EndDrawing();
    }
}

void CollisionDetectionDebugRenderer::debugRenderTriangulatedTerrain(
    const std::vector<SimpleConvexPolygon> &triangles) {
    for (const auto &triangle : triangles) {
        const auto point1 = Vector2(GraphicsUtil::transformPosition(
            Vector2{triangle.getWorldSpaceVertices()[0].x, triangle.getWorldSpaceVertices()[0].y}));
        const auto point2 = Vector2(GraphicsUtil::transformPosition(
            Vector2{triangle.getWorldSpaceVertices()[1].x, triangle.getWorldSpaceVertices()[1].y}));
        const auto point3 = Vector2(GraphicsUtil::transformPosition(
            Vector2{triangle.getWorldSpaceVertices()[2].x, triangle.getWorldSpaceVertices()[2].y}));

        DrawTriangleLines(point1, point2, point3, RAYWHITE);
    }
}

void CollisionDetectionDebugRenderer::debugRenderAABB(const AABB &aabb) {
    std::vector<Vector> aabbCoords{2};
    aabbCoords[0] = GraphicsUtil::transformPosition(Vector2(aabb.getBottomLeft()));
    aabbCoords[1] = GraphicsUtil::transformPosition(Vector2(aabb.getTopRight()));
    const float width = aabbCoords[1].x - aabbCoords[0].x;
    const float height = aabbCoords[0].y - aabbCoords[1].y;
    const Rectangle box{aabbCoords[0].x, aabbCoords[1].y, width, height};
    DrawRectangleLinesEx(box, 5, GREEN);
}

void CollisionDetectionDebugRenderer::debugRenderCollidingTriangle(const SimpleConvexPolygon &triangle) {
    const auto point1 = Vector2(GraphicsUtil::transformPosition(
        Vector2{triangle.getWorldSpaceVertices()[0].x, triangle.getWorldSpaceVertices()[0].y}));
    const auto point2 = Vector2(GraphicsUtil::transformPosition(
        Vector2{triangle.getWorldSpaceVertices()[1].x, triangle.getWorldSpaceVertices()[1].y}));
    const auto point3 = Vector2(GraphicsUtil::transformPosition(
        Vector2{triangle.getWorldSpaceVertices()[2].x, triangle.getWorldSpaceVertices()[2].y}));
    DrawTriangleLines(point1, point2, point3, RED);
}

void CollisionDetectionDebugRenderer::debugRenderTerrainCollisionEdgeVertexAndNormal(
    const DynamicPolygonTerrainCollisionObject &result) {
    if (result.isPolyIncident) {
        const Vector vertexPos =
            GraphicsUtil::transformPosition(Vector2(result.poly->getWorldSpaceVertices()[result.collisionVertexIdx]));
        const Vector edgeStart =
            GraphicsUtil::transformPosition(Vector2(result.triangle->getWorldSpaceVertices()[result.collisionFaceIdx]));
        const Vector edgeEnd = GraphicsUtil::transformPosition(
            Vector2(result.triangle->getWorldSpaceVertices()[(result.collisionFaceIdx + 1) % 3]));
        const Vector normalStart = vertexPos;
        const Vector normalEnd = GraphicsUtil::transformPosition(
            Vector2(result.poly->getWorldSpaceVertices()[result.collisionVertexIdx] + result.collisionDirection));
        DrawCircle(static_cast<int>(vertexPos.x), static_cast<int>(vertexPos.y), 7.0f, BLUE);
        DrawLineEx(Vector2(edgeStart), Vector2(edgeEnd), 5.0, GREEN);
        DrawLineEx(Vector2(normalStart), Vector2(normalEnd), 5.0, RED);
    } else {
        const Vector vertexPos = GraphicsUtil::transformPosition(
            Vector2(result.triangle->getWorldSpaceVertices()[result.collisionVertexIdx]));
        const Vector edgeStart =
            GraphicsUtil::transformPosition(Vector2(result.poly->getWorldSpaceVertices()[result.collisionFaceIdx]));
        const Vector edgeEnd = GraphicsUtil::transformPosition(
            Vector2(result.poly->getWorldSpaceVertices()[(result.collisionFaceIdx + 1) %
                                                         result.poly->getWorldSpaceVertices().size()]));
        const Vector normalStart = vertexPos;
        const Vector normalEnd = GraphicsUtil::transformPosition(
            Vector2(result.triangle->getWorldSpaceVertices()[result.collisionVertexIdx] + result.collisionDirection));
        DrawCircle(static_cast<int>(vertexPos.x), static_cast<int>(vertexPos.y), 7.0f, BLUE);
        DrawLineEx(Vector2(edgeStart), Vector2(edgeEnd), 5.0, GREEN);
        DrawLineEx(Vector2(normalStart), Vector2(normalEnd), 5.0, RED);
    }
}