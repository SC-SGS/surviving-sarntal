//
// Created by felix-roehr on 9/1/24.
//

#include "AxisAlignedBoundingBox.hpp"

AxisAlignedBoundingBox AxisAlignedBoundingBox::merge(AxisAlignedBoundingBox other) const {
    floatType minX = std::fmin(this->minMin.x, other.minMin.x);
    floatType maxX = std::fmax(this->maxMax.x, other.maxMax.x);
    floatType minY = std::fmin(this->minMin.y, other.minMin.y);
    floatType maxY = std::fmax(this->maxMax.y, other.maxMax.y);
    return {{minX, minY}, {maxX, maxY}};
}

bool AxisAlignedBoundingBox::isIn(Vector point) const {
    if (point.x >= this->minMin.x && point.x <= this->maxMax.x && point.y >= this->minMin.y &&
        point.y <= this->maxMax.y) {
        return true;
    }
    return false;
}

bool AxisAlignedBoundingBox::intersects(Line &line) const { return this->isIn(line.start) || this->isIn(line.end); }
