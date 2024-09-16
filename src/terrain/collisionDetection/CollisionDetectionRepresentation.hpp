//
// Created by felix-roehr on 9/10/24.
//

#ifndef SURVIVING_SARNTAL_COLLISIONDETECTIONREPRESENTATION_HPP
#define SURVIVING_SARNTAL_COLLISIONDETECTIONREPRESENTATION_HPP

#include "../points/StaticPolyline.hpp"
#include <algorithm>
#include <memory>

class CollisionDetectionRepresentation {
    using IsSmallerComparator = bool (AxisAlignedBoundingBox::*)(const AxisAlignedBoundingBox &) const;

  public:
    CollisionDetectionRepresentation(std::vector<std::shared_ptr<StaticPolyline>> const &polySections,
                                     AxisAlignedBoundingBox &boundingBox);

    ~CollisionDetectionRepresentation() = default;

    const AxisAlignedBoundingBox &getBoundingBox() const;

    /**
     * Calculates all intersections of this collision detection representation with the given line.
     *
     * @param line
     * @return list of all intersections
     */
    std::vector<std::shared_ptr<Intersection>> calculateIntersections(Line &line);

    /**
     * Calculates the relevant parts of this collision detection representation within the given bounding box.
     *
     * @param boundingBox
     * @return relevant sections of the collision detection representation
     */
    std::vector<std::shared_ptr<StaticPolyline>> calculateRelevantSections(AxisAlignedBoundingBox &boundingBox);

  private:
    std::vector<std::shared_ptr<StaticPolyline>> polylineSectionsSortedT;
    std::vector<std::shared_ptr<StaticPolyline>> polylineSectionsSortedLeftBorder;
    std::vector<std::shared_ptr<StaticPolyline>> polylineSectionsSortedRightBorder;
    AxisAlignedBoundingBox boundingBox;

    void sortLeftBorder();
    void sortRightBorder();

    /**
     * Calculates the relevant parts of this collision detection representation overlapping with the given bounding box
     * in x-Direction. This is done using binary search.
     *
     * @param boundingBox
     * @param sortedSections
     * @return relevant sections of the collision detection representation
     */
    std::vector<std::shared_ptr<StaticPolyline>>
    calculateRelevantSectionsX(const std::vector<std::shared_ptr<StaticPolyline>> &sortedSections,
                               IsSmallerComparator isSmallerComparator, AxisAlignedBoundingBox &boundingBox);
};

#endif // SURVIVING_SARNTAL_COLLISIONDETECTIONREPRESENTATION_HPP
