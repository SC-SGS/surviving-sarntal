//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_STATICPOLYGON_HPP
#define SURVIVING_SARNTAL_STATICPOLYGON_HPP

#include "../spatialDatastructure/AxisAlignedBoundingBox.hpp"
#include "Line.hpp"
#include "StaticPolyObject.hpp"
#include "StaticPolyline.hpp"

class StaticPolygon : public StaticPolyObject {
  public:
    explicit StaticPolygon(std::vector<Vector> &points);
    StaticPolygon(std::vector<Vector> &points, int index);
    ~StaticPolygon() = default;
};

#endif // SURVIVING_SARNTAL_STATICPOLYGON_HPP
