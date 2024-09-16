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
    ~StaticPolygon() = default;

    /**
     * Determines whether the given point is inside this polygon
     *
     * @param point
     * @return
     */
    bool isIn(const Vector &point) const;

    Vector getCenter() const;

  private:
    Vector center = {};
    Vector basepoint = {}; // Point outside the polygon

    void calculateCenter();
};

#endif // SURVIVING_SARNTAL_STATICPOLYGON_HPP
