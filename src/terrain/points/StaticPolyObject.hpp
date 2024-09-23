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
    StaticPolyObject(std::vector<Vector> &points, int index);
    ~StaticPolyObject() = default;

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
     * @param line
     * @return
     */
    std::vector<std::shared_ptr<Intersection>> calculateIntersections(const Line &line) const;

    /**
     * Calculates a List of all intersections of the this poly object with the given one.
     *
     * @param other
     * @return
     */
    std::vector<std::shared_ptr<Intersection>>
    calculateIntersections(const std::shared_ptr<StaticPolyObject> &other) const;

    const virtual std::vector<Vector> &getPoints() const;

    AxisAlignedBoundingBox getBoundingBox() const;

    const std::optional<int> &getIndex() const;

  protected:
    std::vector<Vector> points;
    std::optional<int> index = std::nullopt;
    AxisAlignedBoundingBox boundingBox{};

    void recalculateBoundingBox();
};

#endif // SURVIVING_SARNTAL_STATICPOLYOBJECT_HPP
