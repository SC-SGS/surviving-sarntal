
//
// Created by felix-roehr on 7/26/24.
//

#include "StaticPolygon.hpp"

StaticPolygon::StaticPolygon(std::vector<Vector> &points) : StaticPolyObject(points) {
    Vector delta = {1, 1};
    Vector minMin = points.front();
    Vector maxMax = points.front();
    Vector sum = {0, 0};
    for (auto point : points) {
        if (point.x > maxMax.x) {
            maxMax.x = point.x;
        }
        if (point.y > maxMax.y) {
            maxMax.y = point.y;
        }

        if (point.x < minMin.x) {
            minMin.x = point.x;
        }
        if (point.y < minMin.y) {
            minMin.y = point.y;
        }

        sum += point;
    }
    this->basepoint = maxMax + delta;
    this->boundingBox = {minMin, maxMax};
    this->center = sum / static_cast<floatType>(points.size());
}

bool StaticPolygon::isIn(const Vector &point) const { return false; }
Vector StaticPolygon::getCenter() const { return {}; }
void StaticPolygon::calculateCenter() {}