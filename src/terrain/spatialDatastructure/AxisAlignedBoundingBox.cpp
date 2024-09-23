//
// Created by felix-roehr on 9/1/24.
//

#include "AxisAlignedBoundingBox.hpp"

#include <cmath>

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

AxisAlignedBoundingBox AxisAlignedBoundingBox::moveByDelta(Vector delta) const {
    return {this->minMin + delta, this->maxMax + delta};
}

AxisAlignedBoundingBox AxisAlignedBoundingBox::transform(const Rectangle &rectangle) {
    return {{rectangle.x, rectangle.y}, {rectangle.x + rectangle.width, rectangle.y + rectangle.height}};
}
