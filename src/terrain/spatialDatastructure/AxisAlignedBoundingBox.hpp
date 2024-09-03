//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_AXISALIGNEDBOUNDINGBOX_HPP
#define SURVIVING_SARNTAL_AXISALIGNEDBOUNDINGBOX_HPP

#include "../../utilities/vector.h"
#include "../points/Line.hpp"

struct AxisAlignedBoundingBox {
    Vector minMin;
    Vector maxMax;

    /**
     * Merges this axis aligned bounding box with another one.
     *
     * @param other
     * @return new bounding box, that includes the space of both old ones
     */
    AxisAlignedBoundingBox merge(AxisAlignedBoundingBox other) const;

    /**
     * Checks whether the given point is inside this bounding box.
     *
     * @param point
     * @return
     */
    bool isIn(Vector point) const;

    /**
     * Checks whether the given point intersects this bounding box.
     *
     * @param point
     * @return
     */
    bool intersects(Line &line) const;
};

#endif // SURVIVING_SARNTAL_AXISALIGNEDBOUNDINGBOX_HPP
