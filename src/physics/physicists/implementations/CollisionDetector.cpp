//
// Created by bjoern on 5/21/24.
//

#include "../CollisionDetector.hpp"
#include <mutex>

CollisionDetector::CollisionDetector(World &world) : world(world) {}

void CollisionDetector::detectCollisions() const {}

bool CollisionDetector::rocksCollide(Rock &rock1, Rock &rock2) {
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
};

Vector CollisionDetector::getNormal(const Vector rockPos) const {
    // Vector normal = this->world.getMountain().calculateNormalVector(rockPos.x);
    //  if (normal.y < 0) {
    //      normal = normal * -1.f;
    //  }
    // return normal;
    return {0, 0};
}

CollisionObject CollisionDetector::polygonsCollide(const DynamicPolygon &poly1, const DynamicPolygon &poly2) {
    CollisionObject coll1 = collisionWithSepAxisOn1(poly1, poly2);
    CollisionObject coll2 = collisionWithSepAxisOn1(poly2, poly1);
    if (coll1.collisionDepth < coll2.collisionDepth) {
        return coll1;
    }
    return coll2;
}

std::vector<CollisionObject> CollisionDetector::polygonTerrainCollision(const DynamicPolygon &poly,
                                                                        const Terrain &terrain) {
    const Rectangle rectangle = poly.getBoundingBox();
    const AxisAlignedBoundingBox boundingBox = AxisAlignedBoundingBox::transform(rectangle);
    const auto &mountainSurface = terrain.getTerrainSections(boundingBox);
    std::vector<CollisionObject> collisions{};
    if (mountainSurface.empty()) {
        return collisions;
    }
    Vector first = mountainSurface.front()->getStartPoint();
    Vector second{};
    bool firstCall = true;
    for (const auto &polyline : mountainSurface) {
        for (const auto &point : polyline->getPoints()) {
            if (firstCall) {
                firstCall = false;
                continue;
            }
            second = point;
            const PolygonEdge edge{first, second};
            const CollisionObject collision = polygonLineCollision(poly, edge);
            if (collision.isCollision) {
                collisions.push_back(collision);
            }
            first = second;
        }
    }
    return collisions;
}

CollisionObject CollisionDetector::polygonLineCollision(const DynamicPolygon &poly, const PolygonEdge &edge) {
    const Vector normal = (edge.vertexB - edge.vertexA).getNormalVector();
    const PolygonProjectionOnAxis projPolyOnNormal = projectPolygonOnNormal(poly, normal);
    const floatType projLineOnNormal = edge.vertexA.dot(normal);
    CollisionObject result{};
    if (projLineOnNormal < projPolyOnNormal.lowerBound) {
        result.isCollision = false;
    } else {
        result.isCollision = true;
        result.collisionDepth = projLineOnNormal - projPolyOnNormal.lowerBound;
        result.collisionDirection = normal;
    }
    return result;
}

CollisionObject CollisionDetector::collisionWithSepAxisOn1(const DynamicPolygon &poly1, const DynamicPolygon &poly2) {
    CollisionObject result{};
    const std::vector<Vector> vertices1 = poly1.getWorldSpaceVertices();
    const std::vector<Vector> vertices2 = poly2.getWorldSpaceVertices();
    // Check Edges of Poly1 for separating axis
    for (size_t i = 0; i < vertices1.size(); ++i) {
        const Vector edge = vertices1[(i + 1) % vertices1.size()] - vertices1[i];
        const Vector normal = edge.normalize();
        const PolygonProjectionOnAxis proj1 = projectPolygonOnNormal(poly1, normal);
        const PolygonProjectionOnAxis proj2 = projectPolygonOnNormal(poly2, normal);
        if (proj1.overlaps(proj2)) {
            result.isCollision = true;
            const floatType collisionDepth =
                std::min(proj1.upperBound - proj2.lowerBound, proj2.upperBound - proj1.lowerBound);
            if (result.collisionDepth > collisionDepth) {
                result.collisionDepth = collisionDepth;
                result.collisionDirection = normal;
            }
        }
    }
    return result;
}

PolygonProjectionOnAxis CollisionDetector::projectPolygonOnNormal(const DynamicPolygon &poly, const Vector &normal) {
    floatType epsilon = 0.001;
    PolygonProjectionOnAxis proj{};
    if (normal.length() < 1 - epsilon || normal.length() > 1 + epsilon) {
        throw std::invalid_argument("Normal is not normalized");
    }
    for (const Vector &vertex : poly.getWorldSpaceVertices()) {
        const float projectionLength = vertex.dot(normal);
        proj.lowerBound = std::min(projectionLength, proj.lowerBound);
        proj.upperBound = std::max(projectionLength, proj.upperBound);
    }
    return proj;
}
