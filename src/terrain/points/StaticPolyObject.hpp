//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_STATICPOLYOBJECT_HPP
#define SURVIVING_SARNTAL_STATICPOLYOBJECT_HPP

#include "../../utilities/vector.h"
#include "../spatialDatastructure/AxisAlignedBoundingBox.hpp"
#include "Intersection.hpp"
#include "Line.hpp"
#include <memory>
#include <vector>

class StaticPolyObject {
  public:
    explicit StaticPolyObject(std::vector<Vector> &points);
    ~StaticPolyObject() = default;

    ///**
    // * Initializes a new PolyObject based on the points of the previous one, where all points are shifted by the given
    // * delta.
    // *
    // * @param polyObject
    // * @param delta
    // */
    // StaticPolyObject(StaticPolyObject *polyObject, Vector delta);

    /**
     * Determines whether the given line intersects this poly object
     *
     * @return
     */
    bool intersects(const Line &line) const;

    /**
     * Determines whether the given poly object intersects this poly object
     *
     * @return
     */
    bool intersects(const std::shared_ptr<StaticPolyObject> &other) const;

    /**
     * Calculates a List of all intersections of the given line with this poly object
     *
     * @param start
     * @param end
     * @return
     */
    std::vector<std::shared_ptr<Intersection>> calculateIntersections(const Line &line) const;

    const std::vector<Vector> &getPoints() const;

    AxisAlignedBoundingBox getBoundingBox() const;

  protected:
    std::vector<Vector> points;

    AxisAlignedBoundingBox boundingBox{};

    void recalculateBoundingBox();
};

#endif // SURVIVING_SARNTAL_STATICPOLYOBJECT_HPP
