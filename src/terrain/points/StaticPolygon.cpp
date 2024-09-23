
//
// Created by felix-roehr on 7/26/24.
//

#include "StaticPolygon.hpp"

StaticPolygon::StaticPolygon(std::vector<Vector> &points) : StaticPolyObject(points) {
    this->points.push_back(points.front());
}

StaticPolygon::StaticPolygon(std::vector<Vector> &points, int index) : StaticPolyObject(points, index) {
    this->points.push_back(points.front());
}
