//
// Created by felix-roehr on 7/26/24.
//

#include "StaticPolyObject.hpp"
#include "spdlog/spdlog.h"

StaticPolyObject::StaticPolyObject(std::vector<Vector> &points) : points(points) { this->recalculateBoundingBox(); }

const std::vector<Vector> &StaticPolyObject::getPoints() const { return points; }

std::vector<Intersection> StaticPolyObject::calculateIntersections(const Line &line) const {
    std::vector<Intersection> intersections = {};
    for (int index = 0; index < this->points.size() - 1; index++) {
        Line linePiece = {this->points.at(index), this->points.at(index + 1)};
        std::optional<Intersection> intersection = linePiece.calculateIntersection(line);
        if (intersection.has_value()) {
            intersections.push_back(intersection.value());
        }
    }
    return intersections;
}

bool StaticPolyObject::intersects(const Line &line) const {
    if (this->points.size() < 2) {
        return false;
    }
    for (int index = 0; index < this->points.size() - 1; index++) {
        Vector firstPoint = this->points.at(index);
        Vector secondPoint = this->points.at(index + 1);
        if (Vector::getIntersection(firstPoint, secondPoint, line.start, line.end).has_value()) {
            return true;
        }
    }
    return false;
}

bool StaticPolyObject::intersects(const StaticPolyObject *other) const {
    for (int index = 0; index < other->getPoints().size() - 1; index++) {
        Vector start = other->getPoints().at(index);
        Vector end = other->getPoints().at(index + 1);
        if (this->intersects({start, end})) {
            return true;
        }
    }
    return false;
}

StaticPolyObject::StaticPolyObject(StaticPolyObject *polyObject, Vector delta) {
    std::vector<Vector> newPoints = {};
    for (Vector point : polyObject->getPoints()) {
        newPoints.push_back(point + delta);
    }
    this->points = newPoints;
}

AxisAlignedBoundingBox StaticPolyObject::getBoundingBox() const { return this->boundingBox; }

void StaticPolyObject::recalculateBoundingBox() {
    Vector minMin = points.front();
    Vector maxMax = points.front();
    for (Vector point : points) {
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
    }
    this->boundingBox = {minMin, maxMax};
}
