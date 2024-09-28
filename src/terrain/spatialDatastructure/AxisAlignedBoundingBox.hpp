//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_AXISALIGNEDBOUNDINGBOX_HPP
#define SURVIVING_SARNTAL_AXISALIGNEDBOUNDINGBOX_HPP

#include "../../geometry/AABB.h"
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
    AxisAlignedBoundingBox merge(const AxisAlignedBoundingBox &other) const;

    /**
     * Extends this Bounding box by extending the outline with this point if necessary.
     *
     * @param point
     */
    void extend(const Vector &point);

    /**
     * Checks whether the given point is inside this bounding box.
     *
     * @param point
     * @return
     */
    bool isIn(const Vector &point) const;

    /**
     * Checks whether the given line is completely inside this bounding box.
     *
     * @param line
     * @return
     */
    bool isIn(const Line &line) const;

    /**
     * Checks whether the given line intersects this bounding box.
     *
     * @param line
     * @return
     */
    bool intersects(const Line &line) const;

    /**
     * Checks whether the given line intersects this bounding box or is completely inside it.
     *
     * @param line
     * @return
     */
    bool intersectsOrIsIn(const Line &line) const;

    /**
     * Computes the minimal bounding box surrounding the given line.
     *
     * @param line
     * @return
     */
    static AxisAlignedBoundingBox getBoundingBox(const Line &line);

    /**
     * Computes the minimal bounding box surrounding the given line and adds the tolerance to each direction.
     *
     * @param line
     * @param tolerance
     * @return
     */
    static AxisAlignedBoundingBox getBoundingBoxWithTolerance(const Line &line, floatType tolerance);

    /**
     * Compares the bounding boxes by their borders in x-Direction.
     * This AABB is smaller than the given one, if the left border of this AABB is smaller than the left border of the
     * other.
     *
     * @param other
     * @return
     */
    bool isSmallerLeftBorder(const AxisAlignedBoundingBox &other) const;

    /**
     * Compares the bounding boxes by their borders in x-Direction.
     * This AABB is smaller than the given one, if the right border of this AABB is smaller than the right border of the
     * other.
     *
     * @param other
     * @return
     */
    bool isSmallerRightBorder(const AxisAlignedBoundingBox &other) const;

    /**
     * Compares the bounding boxes by their borders in y-Direction.
     * This AABB is smaller than the given one, if the lower border of this AABB is smaller than the lower border of the
     * other.
     *
     * @param other
     * @return
     */
    bool isSmallerY(const AxisAlignedBoundingBox &other) const;

    /**
     * Compares the bounding boxes by their borders in y-Direction.
     * This AABB is greater than the given one, if the upper border of this AABB is greater than the upper border of the
     * other.
     *
     * @param other
     * @return
     */
    bool isGreaterY(const AxisAlignedBoundingBox &other) const;

    /**
     * Compares the bounding boxes by their borders in x-Direction.
     * This AABB overlaps with the other, if the left border of this AABB is smaller than the left border of the other
     * and the right border of this AABB is greater than the left border of the other AABB OR the right border of this
     * AABB is greater than the right border of the other and the left border of this AABB is smaller than the right
     * border of the other.
     *
     * @param other
     * @return
     */
    bool overlapsX(const AxisAlignedBoundingBox &other) const;

    /**
     * Compares the bounding boxes by their borders in y-Direction.
     * This AABB overlaps with the other, if the lower border of this AABB is smaller than the lower border of the other
     * and the upper border of this AABB is greater than the lower border of the other AABB OR the upper border of this
     * AABB is greater than the upper border of the other and the lower border of this AABB is smaller than the upper
     * border of the other.
     *
     * @param other
     * @return
     */
    bool overlapsY(const AxisAlignedBoundingBox &other) const;

    /**
     * Compares the bounding boxes by their borders in x- and y-Direction.
     *
     * @param other
     * @return true if the bounding boxes overlap in x- and y-direction
     */
    bool overlaps(const AxisAlignedBoundingBox &other) const;

    bool operator==(const AxisAlignedBoundingBox &other) const;

    /**
     * Computes a new bounding box by moving this Bounding box by the specified delta.
     *
     * @param delta
     */
    AxisAlignedBoundingBox moveByDelta(const Vector &delta) const;

    /**
     * Transforms the given rectangle into a bounding box.
     *
     * @param aabb
     * @return
     */
    static AxisAlignedBoundingBox transform(const AABB &aabb);
};

#endif // SURVIVING_SARNTAL_AXISALIGNEDBOUNDINGBOX_HPP
