//
// Created by felix-roehr on 7/26/24.
//

#include "StaticPolyline.hpp"
#include <cassert>

StaticPolyline::StaticPolyline(std::vector<Vector> &points)
    : StaticPolyObject(points), basepoint(points.front()), startPoint(points.front()), endPoint(points.back()) {
    for (Vector point : this->points) {
        if (point.y < this->basepoint.y) {
            this->basepoint = point;
        }
        this->basepoint -= {0, 1};
    }
}

const Vector &StaticPolyline::getStartPoint() const { return startPoint; }

const Vector &StaticPolyline::getEndPoint() const { return endPoint; }

void StaticPolyline::addPoint(Vector point) {
    this->points.push_back(point);
    if (point.y < this->basepoint.y) {
        this->basepoint = point;
    }
    this->basepoint -= {0, 1};
    this->endPoint = point;

    Vector minMin = this->boundingBox.minMin;
    Vector maxMax = this->boundingBox.maxMax;
    if (minMin.x > point.x) {
        minMin.x = point.x;
    }
    if (minMin.y > point.y) {
        minMin.y = point.y;
    }
    if (maxMax.x < point.x) {
        maxMax.x = point.x;
    }
    if (maxMax.y < point.y) {
        maxMax.y = point.y;
    }
    this->boundingBox = {minMin, maxMax};
}

void StaticPolyline::addPolyline(StaticPolyline *polyline) {
    for (Vector point : polyline->getPoints()) {
        this->addPoint(point);
    }
}

bool StaticPolyline::intersectsWithoutFirstPoint(StaticPolyline *other) const {
    assert(this->endPoint == other->startPoint);
    for (int index = 1; index < other->getPoints().size() - 1; index++) {
        Vector start = other->getPoints().at(index);
        Vector end = other->getPoints().at(index + 1);
        if (this->intersects({start, end})) {
            return true;
        }
    }
    return false;
}

void StaticPolyline::removeLastPoints(int count) {
    assert(this->points.size() > count);
    for (int counter = 0; counter < count; counter++) {
        this->points.erase(this->points.cend());
    }
    this->endPoint = this->points.back();
    this->recalculateBoundingBox();
}
