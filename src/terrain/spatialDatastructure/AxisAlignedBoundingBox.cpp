//
// Created by felix-roehr on 9/1/24.
//

#include "AxisAlignedBoundingBox.hpp"
#include "../Terrain.hpp"

#include <cassert>
#include <cmath>
#include <memory>

AxisAlignedBoundingBox AxisAlignedBoundingBox::merge(const AxisAlignedBoundingBox &other) const {
    const floatType minX = std::fmin(this->minMin.x, other.minMin.x);
    const floatType maxX = std::fmax(this->maxMax.x, other.maxMax.x);
    const floatType minY = std::fmin(this->minMin.y, other.minMin.y);
    const floatType maxY = std::fmax(this->maxMax.y, other.maxMax.y);
    return {{minX, minY}, {maxX, maxY}};
}

bool AxisAlignedBoundingBox::isIn(const Vector &point) const {
    if (point.x >= this->minMin.x && point.x <= this->maxMax.x && point.y >= this->minMin.y &&
        point.y <= this->maxMax.y) {
        return true;
    }
    return false;
}

bool AxisAlignedBoundingBox::intersects(const Line &line) const {
    const Line left = {this->minMin, {this->minMin.x, this->maxMax.y}};
    const Line right = {{this->maxMax.x, this->minMin.y}, this->maxMax};
    const Line bottom = {this->minMin, {this->maxMax.x, this->minMin.y}};
    const Line top = {{this->minMin.x, this->maxMax.y}, this->maxMax};

    return left.calculateIntersection(line).has_value() || right.calculateIntersection(line).has_value() ||
           bottom.calculateIntersection(line).has_value() || top.calculateIntersection(line).has_value();
}

bool AxisAlignedBoundingBox::isIn(const Line &line) const { return this->isIn(line.start) && this->isIn(line.end); }

bool AxisAlignedBoundingBox::intersectsOrIsIn(const Line &line) const {
    return this->isIn(line) || this->intersects(line);
}

AxisAlignedBoundingBox AxisAlignedBoundingBox::getBoundingBox(const Line &line) {
    return {{std::fmin(line.start.x, line.end.x), std::fmin(line.start.y, line.end.y)},
            {std::fmax(line.start.x, line.end.x), std::fmax(line.start.y, line.end.y)}};
}

AxisAlignedBoundingBox AxisAlignedBoundingBox::getBoundingBoxWithTolerance(const Line &line,
                                                                           const floatType tolerance) {
    return {{std::fmin(line.start.x, line.end.x) - tolerance, std::fmin(line.start.y, line.end.y) - tolerance},
            {std::fmax(line.start.x, line.end.x) + tolerance, std::fmax(line.start.y, line.end.y) + tolerance}};
}

bool AxisAlignedBoundingBox::isSmallerLeftBorder(const AxisAlignedBoundingBox &other) const {
    return this->minMin.x < other.minMin.x;
}

bool AxisAlignedBoundingBox::isSmallerY(const AxisAlignedBoundingBox &other) const {
    return this->minMin.y < other.minMin.y;
}

bool AxisAlignedBoundingBox::isSmallerRightBorder(const AxisAlignedBoundingBox &other) const {
    return this->maxMax.x < other.maxMax.x;
}

bool AxisAlignedBoundingBox::isGreaterY(const AxisAlignedBoundingBox &other) const {
    return this->maxMax.y > other.maxMax.y;
}

bool AxisAlignedBoundingBox::overlapsX(const AxisAlignedBoundingBox &other) const {
    return (this->minMin.x <= other.minMin.x && this->maxMax.x >= other.minMin.x) || // this overlapping left of other
           (this->maxMax.x >= other.maxMax.x && this->minMin.x <= other.maxMax.x) || // this overlapping right of other
           (this->minMin.x >= other.minMin.x && this->maxMax.x <= other.maxMax.x);   // this inside other
}

bool AxisAlignedBoundingBox::overlapsY(const AxisAlignedBoundingBox &other) const {
    return (this->minMin.y <= other.minMin.y && this->maxMax.y >= other.minMin.y) || // this overlapping bottom of other
           (this->maxMax.y >= other.maxMax.y && this->minMin.y <= other.maxMax.y) || // this overlapping top of other
           (this->minMin.y >= other.minMin.y && this->maxMax.y <= other.maxMax.y);   // this inside other
}

bool AxisAlignedBoundingBox::overlaps(const AxisAlignedBoundingBox &other) const {
    return this->overlapsX(other) && this->overlapsY(other);
}

bool AxisAlignedBoundingBox::operator==(const AxisAlignedBoundingBox &other) const {
    return this->minMin == other.minMin && this->maxMax == other.maxMax;
}

void AxisAlignedBoundingBox::extend(const Vector &point) {
    if (this->maxMax.x < point.x) {
        this->maxMax.x = point.x;
    }
    if (this->maxMax.y < point.y) {
        this->maxMax.y = point.y;
    }
    if (this->minMin.x > point.x) {
        this->minMin.x = point.x;
    }
    if (this->minMin.y > point.y) {
        this->minMin.y = point.y;
    }
}

AxisAlignedBoundingBox AxisAlignedBoundingBox::moveByDelta(const Vector &delta) const {
    return {this->minMin + delta, this->maxMax + delta};
}

AxisAlignedBoundingBox AxisAlignedBoundingBox::transform(const AABB &aabb) {
    return {aabb.getBottomLeft(), aabb.getTopRight()};
}

AxisAlignedBoundingBox AxisAlignedBoundingBox::extend(floatType tolerance) const {
    Vector tolVec = {tolerance, tolerance};
    return {this->minMin - tolVec, this->maxMax + tolVec};
}

std::vector<Line> AxisAlignedBoundingBox::getEdges() const {
    Vector first = this->minMin;
    Vector second = {this->minMin.x, this->maxMax.y}; // {this->maxMax.x, this->minMin.y};
    Vector third = this->maxMax;
    Vector fourth = {this->maxMax.x, this->minMin.y}; // {this->minMin.x, this->maxMax.y};
    std::vector<Line> edges = {Line{first, second}, Line{second, third}, Line{third, fourth}, Line{fourth, first}};
    return edges;
}

std::vector<Vector> AxisAlignedBoundingBox::getCorners() const {
    Vector first = this->minMin;
    Vector second = {this->minMin.x, this->maxMax.y}; // {this->maxMax.x, this->minMin.y};
    Vector third = this->maxMax;
    Vector fourth = {this->maxMax.x, this->minMin.y}; // {this->minMin.x, this->maxMax.y};
    return {first, second, third, fourth};
}

std::vector<std::shared_ptr<Intersection>> AxisAlignedBoundingBox::calculateIntersections(const Line &line) const {
    std::vector<std::shared_ptr<Intersection>> intersections = {};
    for (const auto &aabbLine : this->getEdges()) {
        std::optional<Intersection> intersection = aabbLine.calculateIntersection(line);
        if (intersection.has_value()) {
            intersections.push_back(std::make_shared<Intersection>(intersection.value()));
        }
    }
    return intersections;
}

Vector AxisAlignedBoundingBox::projectOutwards(const Vector &pointOnOther, const Vector &projectionLine) const {
    Vector projDirection = projectionLine.normalize();
    Line intersectionLine = {pointOnOther, pointOnOther + projDirection * this->getWidth() * this->getHeight()};
    std::vector<std::shared_ptr<Intersection>> intersections = this->calculateIntersections(intersectionLine);
    assert(intersections.size() == 1);
    return this->calculateIntersections(intersectionLine).at(0)->intersection;
}

std::optional<Line> AxisAlignedBoundingBox::getEdge(const Vector &point) const {
    const std::vector<Line> &edges = this->getEdges();
    for (const Line &edge : edges) {
        if (AxisAlignedBoundingBox::isOnEdge(point, edge)) {
            return edge;
        }
    }
    return std::nullopt;
}

bool AxisAlignedBoundingBox::isOnEdge(const Vector &point, const Line &edge) {
    if (point.x == edge.start.x && point.x == edge.end.x) {
        if ((point.y >= edge.start.y && point.y <= edge.end.y) || (point.y <= edge.start.y && point.y >= edge.end.y)) {
            return true;
        }
    } else if (point.y == edge.start.y && point.y == edge.end.y) {
        if ((point.x >= edge.start.x && point.x <= edge.end.x) || (point.x <= edge.start.x && point.x >= edge.end.x)) {
            return true;
        }
    }
    return false;
}

floatType AxisAlignedBoundingBox::maxOfHeightLength() const {
    return std::fmax(this->maxMax.y - this->minMin.y, this->maxMax.x - this->minMin.x);
}

floatType AxisAlignedBoundingBox::getHeight() const { return this->maxMax.y - this->minMin.y; }

floatType AxisAlignedBoundingBox::getWidth() const { return this->maxMax.x - this->minMin.x; }

Vector AxisAlignedBoundingBox::calculateFirstIntersection(const Line &line) const {
    std::vector<std::shared_ptr<Intersection>> startIntersections = this->calculateIntersections(line);
    Terrain::sortIntersections(line, startIntersections);
    return startIntersections.front()->intersection;
}

int AxisAlignedBoundingBox::getIndexOfEdge(const Vector &point) const {
    std::vector<Line> edges = this->getEdges();
    for (int index = 0; index < edges.size(); index++) {
        const Line &line = edges.at(index);
        if (AxisAlignedBoundingBox::isOnEdge(point, line)) {
            return index;
        }
    }
}
