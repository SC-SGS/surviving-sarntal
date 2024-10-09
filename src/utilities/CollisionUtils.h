//
// Created by bjoern on 10/8/24.
//

#ifndef COLLISIONUTILS_H
#define COLLISIONUTILS_H

#include "../geometry/ConvexPolygon.h"
#include "../geometry/DynamicConvexPolygon.h"
#include "../geometry/SimpleConvexPolygon.h"
#include "vector.h"
#include <limits>
#include <optional>

constexpr floatType EDGE_TOLERANCE = 1.0f;

struct PolygonProjectionOnAxis {
    floatType lowerBound{std::numeric_limits<floatType>::infinity()};
    floatType upperBound{-std::numeric_limits<floatType>::infinity()};
    size_t indexOfLowestVertex{0};
    size_t indexOfHighestVertex{0};

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

#endif // COLLISIONUTILS_H
