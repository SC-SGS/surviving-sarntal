//
// Created by bjoern on 5/21/24.
//

#include "../CollisionDetector.hpp"

#include "../../../geometry/SimpleConvexPolygon.h"
#include "../../../output/graphics/renderers/CollisionDetectionDebugRenderer.h"
#include "../CollisionHandler.hpp"
#include <poly2tri/poly2tri.h>

CollisionDetector::CollisionDetector(World &world,
                                     const GameConstants &gameConstants,
                                     CollisionDetectionDebugRenderer &collisionRenderer,
                                     const bool devMode)
    : world(world), gameConstants(gameConstants), devMode(devMode), collRenderer(collisionRenderer) {}

DynamicPolygonCollisionObject CollisionDetector::playerRockCollision(Rock &rock) const {
    auto const &hiker = this->world.getHiker();
    return this->dynamicPolygonCollision(*hiker.getCurrentHitbox(), rock);
}

DynamicPolygonCollisionObject CollisionDetector::dynamicPolygonCollision(DynamicConvexPolygon &poly1,
                                                                         DynamicConvexPolygon &poly2) const {

    if (sweptAABBsCollide(poly1, poly2)) {
        if (this->shouldSubstep(poly1, poly2)) {
            return dynamicPolygonCollisionSubstep(poly1, poly2);
        }
        const PolygonCollisionObject collResult = polygonsCollide(
            poly1, poly2, poly1.getLastWitnessTo(poly2.getPolyID()), poly2.getLastWitnessTo(poly1.getPolyID()));

        if (collResult.isCollision) {
            if (this->gameConstants.physicsConstants.debugCDRendering)
                collRenderer.debugRenderRockCollision(
                    createDynamicPolygonCollisionObjectFrom(collResult, poly1, poly2));
            return createDynamicPolygonCollisionObjectFrom(collResult, poly1, poly2);
        }
        if (collResult.polyBReference == &poly1) {
            poly1.updateLastWitness(poly2.getPolyID(), collResult.lastWitnessEdgeIdx);
        } else {
            poly2.updateLastWitness(poly2.getPolyID(), collResult.lastWitnessEdgeIdx);
        }
    }
    return {};
}

bool CollisionDetector::shouldSubstep(const DynamicConvexPolygon &poly1, const DynamicConvexPolygon &poly2) const {
    return (poly1.getLinearMomentum().length() / poly1.getMass() +
            poly2.getLinearMomentum().length() / poly2.getMass()) *
               this->gameConstants.physicsConstants.physicsDeltaT >
           this->gameConstants.rockConstants.minRockSize / 2;
}

bool CollisionDetector::sweptAABBsCollide(const DynamicConvexPolygon &poly1, const DynamicConvexPolygon &poly2) {
    const AABB &aabb1 = poly1.getSweptBoundingBox();
    const AABB &aabb2 = poly2.getSweptBoundingBox();
    return !(aabb1.getRight() < aabb2.getLeft() || aabb2.getRight() < aabb1.getLeft() ||
             aabb1.getTop() < aabb2.getBottom() || aabb2.getTop() < aabb1.getBottom());
}

DynamicPolygonCollisionObject CollisionDetector::dynamicPolygonCollisionSubstep(DynamicConvexPolygon &poly1,
                                                                                DynamicConvexPolygon &poly2) const {
    PolygonCollisionObject collResult;
    floatType relativeTime = 0.0f;
    SimpleConvexPolygon poly1AtT{std::vector<Vector>{}};
    SimpleConvexPolygon poly2AtT{std::vector<Vector>{}};
    // Substep to the first point of collision
    do {
        relativeTime += this->gameConstants.physicsConstants.rockSubstepSize;
        poly1AtT = SimpleConvexPolygon(poly1.interpolateWorldSpaceVerticesBetweenLastAndCurrent(relativeTime));
        poly2AtT = SimpleConvexPolygon(poly2.interpolateWorldSpaceVerticesBetweenLastAndCurrent(relativeTime));
        collResult =
            polygonsCollide(poly1AtT, poly2AtT, poly1.getLastWitnessTo(poly2.getPolyID()),
                            poly2.getLastWitnessTo(poly1.getPolyID()), this->gameConstants.physicsConstants.epsilon);
    } while (!collResult.isCollision && relativeTime < 1 - NUMERIC_EPSILON);

    if (collResult.isCollision) {
        if (this->gameConstants.physicsConstants.debugCDRendering) {
            collRenderer.debugRenderRockCollision(
                createDynamicPolygonCollisionObjectFrom(collResult, poly1, poly2, poly1AtT, poly2AtT, relativeTime));
        }
        return createDynamicPolygonCollisionObjectFrom(collResult, poly1, poly2, poly1AtT, poly2AtT, relativeTime);
    }

    if (collResult.polyBReference == &poly1AtT) {
        poly1.updateLastWitness(poly2.getPolyID(), collResult.lastWitnessEdgeIdx);
    } else {
        poly2.updateLastWitness(poly2.getPolyID(), collResult.lastWitnessEdgeIdx);
    }
    return {};
}

PolygonCollisionObject CollisionDetector::polygonsCollide(ConvexPolygon &poly1,
                                                          ConvexPolygon &poly2,
                                                          const size_t poly1LastWitnessToPoly2,
                                                          const size_t poly2LastWitnessToPoly1,
                                                          const floatType eps) {
    const PolygonCollisionObject coll1 = collisionWithSepAxisOn1(poly1, poly2, poly1LastWitnessToPoly2, eps);
    if (!coll1.isCollision)
        return coll1;
    const PolygonCollisionObject coll2 = collisionWithSepAxisOn1(poly2, poly1, poly2LastWitnessToPoly1, eps);
    if (!coll2.isCollision)
        return coll2;
    if (coll1.collisionDepth < coll2.collisionDepth) {
        return coll1;
    }
    return coll2;
}

DynamicPolygonTerrainCollisionObject CollisionDetector::polygonTerrainCollision(DynamicConvexPolygon &poly) const {
    const AABB aabb = poly.getSweptBoundingBox().extend(this->gameConstants.physicsConstants.epsilon);
    const auto &terrainSurfaceComponents =
        this->world.getTerrain().getTerrainSectionsContinuous({aabb.getBottomLeft(), aabb.getTopRight()});
    if (terrainSurfaceComponents.empty()) {
        return {};
    }
    const std::optional<ConcavePolygon> terrainPoly = getConcaveTerrainPolygonInAABB(terrainSurfaceComponents, aabb);
    if (!terrainPoly.has_value())
        return {};
    std::vector<SimpleConvexPolygon> triangles = getSurfaceTrianglesOfConcaveTerrainPoly(terrainPoly.value());
    if (this->shouldSubstep(poly)) {
        return dynamicPolygonTerrainCollisionSubstep(poly, triangles);
    }
    DynamicPolygonTerrainCollisionObject result{};
    for (auto &triangle : triangles) {
        PolygonCollisionObject tmpResult =
            polygonsCollide(poly, triangle, 0, 0, gameConstants.physicsConstants.epsilon);
        if (tmpResult.isCollision && tmpResult.collisionDepth < result.collisionDepth) {
            result = createDynamicPolygonTerrainCollisionObjectFrom(tmpResult, poly, triangle);
        }
    }
    if (result.isCollision) {
        if (this->gameConstants.physicsConstants.debugCDRendering) {
            collRenderer.debugTerrainCollisionRendering(result, triangles, aabb);
        }
    }
    return result;
}

PolygonCollisionObject CollisionDetector::collisionWithSepAxisOn1(ConvexPolygon &poly1,
                                                                  ConvexPolygon &poly2,
                                                                  const size_t poly1LastWitnessToPoly2,
                                                                  const floatType eps) {
    PolygonCollisionObject result{};
    const std::vector<Vector> &vertices1 = poly1.getWorldSpaceVertices();
    // Check Edges of Poly1 for separating axis
    for (size_t i = poly1LastWitnessToPoly2; i < vertices1.size() + poly1LastWitnessToPoly2; ++i) {
        const Vector edge = vertices1[(i + 1) % vertices1.size()] - vertices1[i % vertices1.size()];
        const Vector normal = edge.getOutwardsNormalVectorForCCWEdge();
        const PolygonProjectionOnAxis proj1 = projectPolygonOnNormal(poly1, normal);
        const PolygonProjectionOnAxis proj2 = projectPolygonOnNormal(poly2, normal);
        if (proj1.overlaps(proj2, eps)) {
            updateCollisionResultAfterOverlapFound(result, proj1, proj2, vertices1, normal, i);
        } else {
            // Separating Axis found
            result.lastWitnessEdgeIdx = i % vertices1.size();
            return {};
        }
    }
    result.polyAIncident = &poly2;
    result.polyBReference = &poly1;
    return result;
}

void CollisionDetector::updateCollisionResultAfterOverlapFound(PolygonCollisionObject &result,
                                                               const PolygonProjectionOnAxis &proj1,
                                                               const PolygonProjectionOnAxis &proj2,
                                                               const std::vector<Vector> &vertices1,
                                                               const Vector &normal,
                                                               const size_t collisionFaceIdx) {
    result.isCollision = true;
    const bool normalNeedsToBeReversed = proj1.upperBound - proj2.lowerBound > proj2.upperBound - proj1.lowerBound;
    const floatType collisionDepth = std::min(proj1.upperBound - proj2.lowerBound, proj2.upperBound - proj1.lowerBound);
    if (collisionDepth < result.collisionDepth) {
        if (normalNeedsToBeReversed) {
            result.collisionDirection = normal * -1.f;
            result.collisionVertexIdx = proj2.indexOfHighestVertex;
        } else {
            result.collisionDirection = normal;
            result.collisionVertexIdx = proj2.indexOfLowestVertex;
        }
        result.collisionDepth = collisionDepth;
        result.collisionFaceIdx = collisionFaceIdx % vertices1.size();
    }
}

PolygonProjectionOnAxis CollisionDetector::projectPolygonOnNormal(const ConvexPolygon &poly, const Vector &normal) {
    PolygonProjectionOnAxis proj{};
    if (std::abs(1 - normal.lengthSq()) > NUMERIC_EPSILON) {
        throw std::invalid_argument("Normal is not normalized");
    }
    const auto &vertices = poly.getWorldSpaceVertices();
    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto &vertex = vertices[i];
        const floatType projectionLength = vertex.dot(normal);
        if (projectionLength < proj.lowerBound) {
            proj.lowerBound = projectionLength;
            proj.indexOfLowestVertex = i;
        }
        if (projectionLength > proj.upperBound) {
            proj.upperBound = projectionLength;
            proj.indexOfHighestVertex = i;
        }
    }
    return proj;
}

DynamicPolygonCollisionObject CollisionDetector::createDynamicPolygonCollisionObjectFrom(
    const PolygonCollisionObject &collResult, DynamicConvexPolygon &poly1, DynamicConvexPolygon &poly2) {
    DynamicPolygonCollisionObject dynamicResult{
        true,    collResult.collisionDepth,     collResult.collisionDirection, nullptr,
        nullptr, collResult.collisionVertexIdx, collResult.collisionFaceIdx,   1.0f};
    dynamicResult.polyAIncident = collResult.polyAIncident == &poly1 ? &poly1 : &poly2;
    dynamicResult.polyBReference = collResult.polyBReference == &poly2 ? &poly2 : &poly1;
    return dynamicResult;
}

DynamicPolygonCollisionObject
CollisionDetector::createDynamicPolygonCollisionObjectFrom(const PolygonCollisionObject &collResult,
                                                           DynamicConvexPolygon &poly1,
                                                           DynamicConvexPolygon &poly2,
                                                           const ConvexPolygon &poly1AtT,
                                                           const ConvexPolygon &poly2AtT,
                                                           const floatType relativeTime) {
    DynamicPolygonCollisionObject dynamicResult{
        true,    collResult.collisionDepth,     collResult.collisionDirection, nullptr,
        nullptr, collResult.collisionVertexIdx, collResult.collisionFaceIdx,   relativeTime};
    dynamicResult.polyAIncident = collResult.polyAIncident == &poly1AtT ? &poly1 : &poly2;
    dynamicResult.polyBReference = collResult.polyBReference == &poly2AtT ? &poly2 : &poly1;
    return dynamicResult;
}

bool CollisionDetector::shouldSubstep(const DynamicConvexPolygon &poly) const {
    return (poly.getLinearMomentum().length() / poly.getMass()) * this->gameConstants.physicsConstants.physicsDeltaT >
           this->gameConstants.rockConstants.minRockSize / 4;
}

DynamicPolygonTerrainCollisionObject
CollisionDetector::dynamicPolygonTerrainCollisionSubstep(DynamicConvexPolygon &poly,
                                                         std::vector<SimpleConvexPolygon> &triangles) const {
    DynamicPolygonTerrainCollisionObject result{};
    floatType relativeTime = 0.0f;
    do {
        relativeTime += this->gameConstants.physicsConstants.terrainSubstepSize;
        SimpleConvexPolygon polyAtT(poly.interpolateWorldSpaceVerticesBetweenLastAndCurrent(relativeTime));
        result.collisionDepth = std::numeric_limits<floatType>::max(); // Initialize with max depth
        for (auto &triangle : triangles) {
            PolygonCollisionObject tmpResult =
                polygonsCollide(polyAtT, triangle, 0, 0, gameConstants.physicsConstants.epsilon);
            if (tmpResult.isCollision && tmpResult.collisionDepth < result.collisionDepth) {
                result =
                    createDynamicPolygonTerrainCollisionObjectFrom(tmpResult, poly, polyAtT, triangle, relativeTime);
            }
        }
    } while (!result.isCollision && relativeTime < 1 - NUMERIC_EPSILON);
    if (result.isCollision)
        if (this->gameConstants.physicsConstants.debugCDRendering)
            collRenderer.debugTerrainCollisionRendering(
                result, triangles, poly.getSweptBoundingBox().extend(this->gameConstants.physicsConstants.epsilon));
    return result;
}

StaticPolyline CollisionDetector::getContinuousTerrainSurfaceFromComponents(
    const std::vector<std::shared_ptr<StaticPolyline>> &terrainSurfaceComponents) {
    std::vector<Vector> points{};
    for (size_t componentIdx = 0; componentIdx < terrainSurfaceComponents.size(); ++componentIdx) {
        const auto &componentPoints = terrainSurfaceComponents[componentIdx].get()->getPoints();
        if (componentIdx < terrainSurfaceComponents.size() - 1) {
            assert(componentPoints[componentPoints.size() - 1] ==
                   terrainSurfaceComponents[componentIdx + 1].get()->getPoints()[0]);
        }
        for (size_t ptIdx = 0; ptIdx < componentPoints.size() - 1; ++ptIdx) {
            points.push_back(componentPoints[ptIdx]);
        }
        if (componentIdx == terrainSurfaceComponents.size() - 1) {
            points.push_back(componentPoints[componentPoints.size() - 1]);
        }
    }
    return StaticPolyline{points};
}

std::optional<ConcavePolygon> CollisionDetector::getConcaveTerrainPolygonInAABB(
    const std::vector<std::shared_ptr<StaticPolyline>> &terrainSurfaceComponents, const AABB &aabb) {
    const AxisAlignedBoundingBox searchAABB = AxisAlignedBoundingBox::transform(aabb);
    const std::optional<StaticPolyline> &terrainSurface =
        removeRedundantPoints(getContinuousTerrainSurfaceFromComponents(terrainSurfaceComponents), searchAABB);
    if (!terrainSurface.has_value())
        return std::nullopt;
    const std::vector<Vector> &terrainSurfacePoints = terrainSurface.value().getPoints();
    const AxisAlignedBoundingBox &terrainAABB = terrainSurface.value().getBoundingBox();
    const AxisAlignedBoundingBox completeAABB = terrainAABB.merge(searchAABB).extend(EDGE_TOLERANCE);
    assert(terrainSurfacePoints.size() >= 2);
    const Line start = {terrainSurfacePoints.front(), terrainSurfacePoints.at(1)};
    const Line end = {terrainSurfacePoints.back(), terrainSurfacePoints.at(terrainSurfacePoints.size() - 2)};

    Vector entryPoint = searchAABB.calculateFirstIntersection(start);
    Vector exitPoint = searchAABB.calculateFirstIntersection(end);

    const int indexExitLine = searchAABB.getIndexOfEdge(exitPoint);
    const Vector entryPointProjectionLine = searchAABB.getEdge(entryPoint).value().calculateNormal();
    entryPoint = completeAABB.projectOutwards(terrainSurfacePoints.front(), entryPointProjectionLine);
    const Vector exitPointProjectionLine = searchAABB.getEdge(exitPoint).value().calculateNormal();
    exitPoint = completeAABB.projectOutwards(terrainSurfacePoints.back(), exitPointProjectionLine);
    if (entryPoint == exitPoint)
        return std::nullopt;
    return getConcaveTerrainPolygonInAABB(terrainSurfacePoints, completeAABB, entryPoint, exitPoint, indexExitLine);
}

std::optional<ConcavePolygon>
CollisionDetector::getConcaveTerrainPolygonInAABB(const std::vector<Vector> &terrainSurfacePoints,
                                                  const AxisAlignedBoundingBox &completeAABB,
                                                  const Vector &entryPoint,
                                                  const Vector &exitPoint,
                                                  const int indexExitLine) {
    std::vector vertices = {entryPoint};
    vertices.insert(vertices.cend(), terrainSurfacePoints.cbegin(), terrainSurfacePoints.cend());
    vertices.push_back(exitPoint);

    const std::vector<Line> completeAABBEdges = completeAABB.getEdges();
    unsigned long index = indexExitLine;
    Vector previousPoint = vertices.back();
    Vector nextPoint = completeAABBEdges.at(index).end;
    const Vector lastPoint = vertices.front();
    while (!AxisAlignedBoundingBox::isOnEdge(lastPoint, Line{previousPoint, nextPoint})) {
        if (nextPoint != lastPoint) {
            vertices.push_back(nextPoint);
        }
        previousPoint = nextPoint;
        index = (index + 1) % completeAABBEdges.size();
        nextPoint = completeAABBEdges.at(index).end;
    }
    std::reverse(vertices.begin(), vertices.end());
    return ConcavePolygon(vertices);
}

std::vector<SimpleConvexPolygon>
CollisionDetector::getSurfaceTrianglesOfConcaveTerrainPoly(const ConcavePolygon &terrainPoly) {
    const auto &terrainPolyVertices = terrainPoly.getWorldSpaceVertices();
    assert(terrainPolyVertices.size() > 2);
    std::vector<p2t::Point *> terrainPolyLine{terrainPolyVertices.size()};
    for (size_t i = 0; i < terrainPolyVertices.size(); ++i) {
        terrainPolyLine[i] = new p2t::Point(terrainPolyVertices[i].x, terrainPolyVertices[i].y);
    }
    p2t::CDT cdt(terrainPolyLine);
    cdt.Triangulate();
    std::vector<p2t::Triangle *> triangles = cdt.GetTriangles();
    std::vector<SimpleConvexPolygon> result{};
    for (const auto p2tTri : triangles) {
        bool isBoundaryTriangle = false;
        for (const auto constrainedEdge : p2tTri->constrained_edge) {
            if (constrainedEdge) {
                isBoundaryTriangle = true;
                break;
            }
        }
        if (isBoundaryTriangle && p2tTri->IsInterior()) {
            SimpleConvexPolygon triPoly = createConvexPolyFromP2TTriangle(*p2tTri);
            result.push_back(triPoly);
        }
    }
    return result;
}

std::vector<SimpleConvexPolygon> CollisionDetector::triagulateConcaveTerrainPoly(const ConcavePolygon &terrainPoly) {
    const auto &terrainPolyVertices = terrainPoly.getWorldSpaceVertices();
    assert(terrainPolyVertices.size() > 2);
    std::vector<p2t::Point *> terrainPolyLine{terrainPolyVertices.size()};
    for (size_t i = 0; i < terrainPolyVertices.size(); ++i) {
        terrainPolyLine[i] = new p2t::Point(terrainPolyVertices[i].x, terrainPolyVertices[i].y);
    }
    p2t::CDT cdt(terrainPolyLine);
    cdt.Triangulate();
    std::vector<p2t::Triangle *> triangles = cdt.GetTriangles();
    std::vector<SimpleConvexPolygon> result{};
    for (const auto p2tTri : triangles) {
        if (p2tTri->IsInterior()) {
            SimpleConvexPolygon triPoly = createConvexPolyFromP2TTriangle(*p2tTri);
            result.push_back(triPoly);
        }
    }
    return result;
}

SimpleConvexPolygon CollisionDetector::createConvexPolyFromP2TTriangle(p2t::Triangle &p2tTri) {
    std::vector<p2t::Point *> triPoints{3};
    std::vector<Vector> triVertices{3};
    triPoints[0] = p2tTri.GetPoint(0);
    triPoints[1] = p2tTri.PointCCW(*triPoints[0]);
    triPoints[2] = p2tTri.PointCCW(*triPoints[1]);
    for (char i = 0; i < 3; ++i) {
        const p2t::Point *point = triPoints[i];
        triVertices[i] = Vector{static_cast<floatType>(point->x), static_cast<floatType>(point->y)};
    }
    return SimpleConvexPolygon(triVertices);
}

DynamicPolygonTerrainCollisionObject CollisionDetector::createDynamicPolygonTerrainCollisionObjectFrom(
    const PolygonCollisionObject &collision, DynamicConvexPolygon &poly, SimpleConvexPolygon &triangle) {
    DynamicPolygonTerrainCollisionObject result{collision.isCollision,
                                                collision.collisionDepth,
                                                collision.collisionDirection,
                                                &poly,
                                                triangle,
                                                false,
                                                collision.collisionVertexIdx,
                                                collision.collisionFaceIdx,
                                                1.0f};
    if (collision.polyAIncident == &poly) {
        result.isPolyIncident = true;
    }
    return result;
}

DynamicPolygonTerrainCollisionObject
CollisionDetector::createDynamicPolygonTerrainCollisionObjectFrom(const PolygonCollisionObject &collision,
                                                                  DynamicConvexPolygon &poly,
                                                                  const SimpleConvexPolygon &polyAtT,
                                                                  const SimpleConvexPolygon &triangle,
                                                                  const floatType relativeTime) {
    DynamicPolygonTerrainCollisionObject result{collision.isCollision,
                                                collision.collisionDepth,
                                                collision.collisionDirection,
                                                &poly,
                                                triangle,
                                                false,
                                                collision.collisionVertexIdx,
                                                collision.collisionFaceIdx,
                                                relativeTime};
    if (collision.polyAIncident == &polyAtT) {
        result.isPolyIncident = true;
    }
    return result;
}

std::optional<StaticPolyline> CollisionDetector::removeRedundantPoints(const StaticPolyline &terrainSection,
                                                                       const AxisAlignedBoundingBox &searchAABB) {
    const std::vector<Vector> &oldPoints = terrainSection.getPoints();
    std::vector<Vector> newPoints = {};
    int minIndex = 0;
    while (minIndex < oldPoints.size() - 1 &&
           !searchAABB.intersectsOrIsIn(Line{oldPoints.at(minIndex), oldPoints.at(minIndex + 1)})) {
        minIndex++;
    }
    int maxIndex = static_cast<int>(oldPoints.size()) - 1;
    while (maxIndex > 0 && !searchAABB.intersectsOrIsIn(Line{oldPoints.at(maxIndex), oldPoints.at(maxIndex - 1)})) {
        maxIndex--;
    }
    for (int index = minIndex; index <= maxIndex; index++) {
        newPoints.push_back(oldPoints.at(index));
    }
    if (newPoints.empty()) {
        return std::nullopt;
    }
    return StaticPolyline(newPoints);
}
