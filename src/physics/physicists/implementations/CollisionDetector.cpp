//
// Created by bjoern on 5/21/24.
//

#include "../CollisionDetector.hpp"

#include "../../../geometry/SimpleConvexPolygon.h"
#include "../../../output/graphics/renderers/CollisionDetectionDebugRenderer.h"
#include "../CollisionHandler.hpp"
#include <poly2tri/poly2tri.h>

CollisionDetector::CollisionDetector(World &world, const GameConstants &gameConstants, const bool devMode)
    : world(world), gameConstants(gameConstants), devMode(devMode) {}

void CollisionDetector::detectCollisions() const {}

/*bool CollisionDetector::rocksCollide(Rock &rock1, Rock &rock2) {
    const Vector pos1 = rock1.getPosition();
    const floatType rad1 = rock1.getBoundingBox().width / 2;
    const Vector pos2 = rock2.getPosition();
    const floatType rad2 = rock2.getBoundingBox().width / 2;
    return (pos1.distanceTo(pos2) <= rad1 + rad2);
}

bool CollisionDetector::isPlayerHitByRock(Rock &rock) {
    const auto rockPosition = rock.getPosition();
    const auto rad = rock.getBoundingBox().width / 2;
    // hiker position is at his feet, but here we use the center of mass (center of the rectangle) as position
    const auto pos = this->world.getHiker().getPosition();
    const auto adjustedHikerPosition = Vector{pos.x, pos.y + this->world.getHiker().getHeight() / 2};
    const auto hikerHeight = this->world.getHiker().getHeight();
    const auto hikerWidth = this->world.getHiker().getWidth();
    const auto xCenterDistanceAbs = std::abs(adjustedHikerPosition.x - rockPosition.x);
    const auto yCenterDistanceAbs = std::abs(adjustedHikerPosition.y - rockPosition.y);
    bool isHit = false;

    if (xCenterDistanceAbs > hikerWidth / 2 + rad || yCenterDistanceAbs > hikerHeight / 2 + rad) {

    } else if (xCenterDistanceAbs <= hikerWidth / 2 || yCenterDistanceAbs <= hikerHeight / 2) {
        isHit = true;
    } else {
        const auto cornerDistanceSq =
            std::pow(xCenterDistanceAbs - hikerWidth / 2, 2) + std::pow(yCenterDistanceAbs - hikerHeight / 2, 2);
        if (cornerDistanceSq <= std::pow(rad, 2)) {
            isHit = true;
        }
    }
    return isHit;
};*/

DynamicPolygonCollisionObject CollisionDetector::dynamicPolygonCollision(DynamicConvexPolygon &poly1,
                                                                         DynamicConvexPolygon &poly2) const {

    if (sweptAABBsCollide(poly1, poly2)) {
        PolygonCollisionObject collResult;
        floatType relativeTime = 0.0f; // TODO better choice of smallDT needed for bullets dependent on velocity
        SimpleConvexPolygon poly1AtT{std::vector<Vector>{}};
        SimpleConvexPolygon poly2AtT{std::vector<Vector>{}};
        // Substep to the first point of collision
        do {
            relativeTime += this->gameConstants.physicsConstants.rockSubstepSize;
            poly1AtT = SimpleConvexPolygon(poly1.interpolateWorldSpaceVerticesBetweenLastAndCurrent(relativeTime));
            poly2AtT = SimpleConvexPolygon(poly2.interpolateWorldSpaceVerticesBetweenLastAndCurrent(relativeTime));
        } while (!(collResult = polygonsCollide(poly1AtT, poly2AtT, poly1.getLastWitnessTo(poly2.getID()),
                                                poly2.getLastWitnessTo(poly1.getID()),
                                                this->gameConstants.physicsConstants.epsilon))
                      .isCollision &&
                 relativeTime < 0.999);
        if (collResult.isCollision) {
            if (this->devMode && this->gameConstants.physicsConstants.debugCDRendering)
                CollisionDetectionDebugRenderer::debugRenderRockCollision(createDynamicPolygonCollisionObjectFrom(
                    collResult, poly1, poly2, poly1AtT, poly2AtT, relativeTime));
            return createDynamicPolygonCollisionObjectFrom(collResult, poly1, poly2, poly1AtT, poly2AtT, relativeTime);
        }
        if (collResult.polyBReference == &poly1AtT) {
            poly1.updateLastWitness(poly2.getID(), collResult.lastWitnessEdgeIdx);
        } else {
            poly2.updateLastWitness(poly2.getID(), collResult.lastWitnessEdgeIdx);
        }
    }
    return {};
}

bool CollisionDetector::sweptAABBsCollide(const DynamicConvexPolygon &poly1, const DynamicConvexPolygon &poly2) {
    AABB aabb1 = poly1.getSweptBoundingBox();
    AABB aabb2 = poly2.getSweptBoundingBox();
    const PolygonCollisionObject aabbColl = polygonsCollide(aabb1, aabb2);
    return aabbColl.isCollision;
}

PolygonCollisionObject CollisionDetector::polygonsCollide(ConvexPolygon &poly1,
                                                          ConvexPolygon &poly2,
                                                          const size_t poly1LastWitnessToPoly2,
                                                          const size_t poly2LastWitnessToPoly1,
                                                          const floatType eps) {
    const PolygonCollisionObject coll1 = collisionWithSepAxisOn1(poly1, poly2, poly1LastWitnessToPoly2, eps);
    const PolygonCollisionObject coll2 = collisionWithSepAxisOn1(poly2, poly1, poly2LastWitnessToPoly1, eps);
    if (!coll1.isCollision)
        return coll1;
    if (!coll2.isCollision)
        return coll2;
    if (coll1.collisionDepth < coll2.collisionDepth) {
        return coll1;
    }
    return coll2;
}

DynamicPolygonTerrainCollisionObject CollisionDetector::polygonTerrainCollision(DynamicConvexPolygon &poly) const {
    const AABB aabb = poly.getSweptBoundingBox();
    const auto &terrainSurfaceComponents =
        this->world.getTerrain().getTerrainSectionsContinuous({aabb.getBottomLeft(), aabb.getTopRight()});
    if (terrainSurfaceComponents.empty()) {
        return {};
    }
    const ConcavePolygon terrainPoly = getConcaveTerrainPolygonInAABB(terrainSurfaceComponents, aabb);
    std::vector<SimpleConvexPolygon> triangles = getSurfaceTrianglesOfConcaveTerrainPoly(terrainPoly);
    DynamicPolygonTerrainCollisionObject result{};
    floatType relativeTime = 0.0f; // TODO better choice of smallDT needed for bullets dependent on velocity
    do {
        relativeTime += this->gameConstants.physicsConstants.terrainSubstepSize;
        SimpleConvexPolygon polyAtT(poly.interpolateWorldSpaceVerticesBetweenLastAndCurrent(relativeTime));
        for (auto &triPoly : triangles) {
            PolygonCollisionObject tmpResult =
                polygonsCollide(polyAtT, triPoly, 0, 0, this->gameConstants.physicsConstants.epsilon);
            if (tmpResult.isCollision && tmpResult.collisionDepth < result.collisionDepth) {
                result =
                    createDynamicPolygonTerrainCollisionObjectFrom(tmpResult, poly, polyAtT, triPoly, relativeTime);
            }
        }
    } while (!result.isCollision && relativeTime < 0.999);
    if (result.isCollision)
        if (this->devMode && this->gameConstants.physicsConstants.debugCDRendering)
            CollisionDetectionDebugRenderer::debugTerrainCollisionRendering(result, triangles, aabb);
    spdlog::debug("Terrain Collision");
    return result;
}

PolygonCollisionObject CollisionDetector::collisionWithSepAxisOn1(ConvexPolygon &poly1,
                                                                  ConvexPolygon &poly2,
                                                                  const size_t poly1LastWitnessToPoly2,
                                                                  const floatType eps) {
    PolygonCollisionObject result{};
    const std::vector<Vector> vertices1 = poly1.getWorldSpaceVertices();
    // Check Edges of Poly1 for separating axis
    for (size_t i = poly1LastWitnessToPoly2; i < vertices1.size() + poly1LastWitnessToPoly2; ++i) {
        const Vector edge = vertices1[(i + 1) % vertices1.size()] - vertices1[i % vertices1.size()];
        const Vector normal = edge.getOutwardsNormalVectorForCCWEdge();
        const PolygonProjectionOnAxis proj1 = projectPolygonOnNormal(poly1, normal);
        const PolygonProjectionOnAxis proj2 = projectPolygonOnNormal(poly2, normal);
        if (proj1.overlaps(proj2, eps)) {
            result.isCollision = true;
            const floatType normalNeedsToBeReversed =
                proj1.upperBound - proj2.lowerBound > proj2.upperBound - proj1.lowerBound ? -1 : 1;
            const floatType collisionDepth =
                std::min(proj1.upperBound - proj2.lowerBound, proj2.upperBound - proj1.lowerBound);
            if (collisionDepth < result.collisionDepth) {
                result.collisionDepth = collisionDepth;
                result.collisionDirection = normal * normalNeedsToBeReversed;
                result.collisionFaceIdx = i % vertices1.size();
                result.collisionVertexIdx = proj2.indexOfLowestVertex;
            }
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

PolygonProjectionOnAxis CollisionDetector::projectPolygonOnNormal(const ConvexPolygon &poly, const Vector &normal) {
    PolygonProjectionOnAxis proj{};
    if (std::abs(1 - normal.length()) > NUMERIC_EPSILON) {
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
        proj.lowerBound = std::min(projectionLength, proj.lowerBound);
        proj.upperBound = std::max(projectionLength, proj.upperBound);
    }
    return proj;
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

ConcavePolygon CollisionDetector::getConcaveTerrainPolygonInAABB(
    const std::vector<std::shared_ptr<StaticPolyline>> &terrainSurfaceComponents, const AABB &aabb) {
    const StaticPolyline &terrainSurface = getContinuousTerrainSurfaceFromComponents(terrainSurfaceComponents);
    const std::vector<Vector> &terrainSurfacePoints = terrainSurface.getPoints();
    const AxisAlignedBoundingBox terrainAABB = terrainSurface.getBoundingBox();
    const Vector leftOuter{terrainAABB.minMin.x - aabb.getWidth(), terrainSurfacePoints[0].y};
    const Vector rightOuter{terrainAABB.maxMax.x + aabb.getWidth(),
                            terrainSurfacePoints[terrainSurfacePoints.size() - 1].y};
    const Vector bottomLeft{terrainAABB.minMin.x - aabb.getWidth(), terrainAABB.minMin.y - aabb.getHeight()};
    const Vector bottomRight{terrainAABB.maxMax.x + aabb.getWidth(), terrainAABB.minMin.y - aabb.getHeight()};
    /* TODO this can be refined to get a smaller polygon that fits the AABB better in order to check less edges
    for (size_t i = 0; i < mountainSurfacePoints.size() - 1; ++i) {
        const PolygonEdge line{mountainSurfacePoints[i], mountainSurfacePoints[i + 1]};
        switch (lineIntersectsAABB(line, aabb)) {
            case LEF
        }
    }*/
    std::vector<Vector> vertices{};
    vertices.push_back(leftOuter);
    vertices.push_back(bottomLeft);
    vertices.push_back(bottomRight);
    vertices.push_back(rightOuter);
    std::reverse_copy(terrainSurfacePoints.begin(), terrainSurfacePoints.end(), std::back_inserter(vertices));
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

DynamicPolygonTerrainCollisionObject
CollisionDetector::createDynamicPolygonTerrainCollisionObjectFrom(const PolygonCollisionObject &collision,
                                                                  DynamicConvexPolygon &poly,
                                                                  const SimpleConvexPolygon &polyAtT,
                                                                  const ConvexPolygon &triangle,
                                                                  const floatType relativeTime) {
    DynamicPolygonTerrainCollisionObject result{collision.isCollision,
                                                collision.collisionDepth,
                                                collision.collisionDirection,
                                                &poly,
                                                SimpleConvexPolygon{triangle.getWorldSpaceVertices()},
                                                false,
                                                collision.collisionVertexIdx,
                                                collision.collisionFaceIdx,
                                                relativeTime};
    if (collision.polyAIncident == &polyAtT) {
        result.isPolyIncident = true;
    }
    return result;
}
