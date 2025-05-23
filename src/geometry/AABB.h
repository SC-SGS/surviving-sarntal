//
// Created by bjoern on 9/19/24.
//

#ifndef AABB_H
#define AABB_H
#include "../utilities/vector.h"
#include "ConvexPolygon.h"

class AABB final : public ConvexPolygon {
    std::vector<Vector> worldSpaceCoordinates{};

  public:
    AABB(const Vector &bottomLeft, const Vector &topRight);
    const std::vector<Vector> &getWorldSpaceVertices() const override { return this->worldSpaceCoordinates; }
    Vector getTopRight() const { return this->worldSpaceCoordinates[2]; }
    Vector getBottomLeft() const { return this->worldSpaceCoordinates[0]; }
    floatType getWidth() const { return this->worldSpaceCoordinates[1].x - this->worldSpaceCoordinates[0].x; }
    floatType getHeight() const { return this->worldSpaceCoordinates[2].y - this->worldSpaceCoordinates[0].y; }
    AABB extend(const floatType tolerance) const {
        const Vector tolVec = {tolerance, tolerance};
        return {this->getBottomLeft() - tolVec, this->getTopRight() + tolVec};
    }
    floatType getLeft() const { return this->worldSpaceCoordinates[0].x; }
    floatType getRight() const { return this->worldSpaceCoordinates[2].x; }
    floatType getBottom() const { return this->worldSpaceCoordinates[0].y; }
    floatType getTop() const { return this->worldSpaceCoordinates[2].y; }
};

inline AABB::AABB(const Vector &bottomLeft, const Vector &topRight) {
    this->worldSpaceCoordinates.push_back(bottomLeft);
    this->worldSpaceCoordinates.push_back({topRight.x, bottomLeft.y});
    this->worldSpaceCoordinates.push_back(topRight);
    this->worldSpaceCoordinates.push_back({bottomLeft.x, topRight.y});
}

#endif // AABB_H
