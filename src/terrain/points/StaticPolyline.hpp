//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_STATICPOLYLINE_HPP
#define SURVIVING_SARNTAL_STATICPOLYLINE_HPP

#include "../../utilities/vector.h"
#include "StaticPolyObject.hpp"

#include <vector>

class StaticPolyline : public StaticPolyObject {
  public:
    explicit StaticPolyline(std::vector<Vector> &points);
    ~StaticPolyline() = default;

    /**
     * Adds a point to the end of this polyline. The previously last point connects to this.
     * @param point
     */
    void addPoint(Vector point);

    /**
     * Adds all points from the given polyline to the end of this one.
     * @param polyline
     */
    void addPolyline(StaticPolyline *polyline);

    /**
     * Checks whether the given polyline intersects this one.
     * The start of the given polyline needs to match the end of this one. This shared point is not counted as an
     * intersection.
     *
     * @param other
     * @return
     */
    bool intersectsWithoutFirstPoint(StaticPolyline *other) const;

    const Vector &getStartPoint() const;
    const Vector &getEndPoint() const;

    /**
     * removes the last count points from this polyline.
     *
     * @param count
     */
    void removeLastPoints(int count);

  private:
    Vector basepoint; // Point below the polyline
    Vector startPoint;
    Vector endPoint;
};

#endif // SURVIVING_SARNTAL_STATICPOLYLINE_HPP
