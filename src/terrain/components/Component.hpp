//
// Created by felix-roehr on 8/5/24.
//

#ifndef SURVIVING_SARNTAL_COMPONENT_HPP
#define SURVIVING_SARNTAL_COMPONENT_HPP

#include "../collisionDetection/CollisionDetectionRepresentation.hpp"
#include "../points/StaticPolyObject.hpp"
#include "../points/StaticPolyline.hpp"
#include <memory>

template <typename U, typename = std::enable_if_t<std::is_base_of_v<StaticPolyObject, U>>> class Component {
  public:
    explicit Component(std::shared_ptr<U> polyObject, TerrainConstants const &terrainConstants)
        : polyObject(polyObject),
          startPoint(polyObject->getPoints().front()),
          endPoint(polyObject->getPoints().back()),
          terrainConstants(terrainConstants){};

    /**
     * Returns a list of polylines between the basepoints of this component with the specified resolution
     *
     * @param resolution the step size for the parameter t. This is roughly equivalent to the euclidean distance of the
     * points in the resulting poly-object.
     * @return the poly-object
     */
    std::shared_ptr<CollisionDetectionRepresentation>
    calculateCollisionDetectionRepresentation(floatType resolution) const {
        std::vector<std::shared_ptr<StaticPolyline>> pointSections = {};
        AxisAlignedBoundingBox boundingBox = {this->polyObject->getPoints().front(),
                                              this->polyObject->getPoints().front()};
        floatType relativeT;
        for (int index = 0; index < this->polyObject->getPoints().size() - 1; index++) {
            std::vector<Vector> points = {this->polyObject->getPoints().at(index)};
            relativeT = this->relativeTs.at(index) + resolution;
            while (relativeT < this->relativeTs.at(index + 1)) {
                Vector point = this->evaluate(relativeT);
                boundingBox.extend(point);
                points.push_back(point);
                relativeT += resolution;
            }
            Vector endPoint = this->polyObject->getPoints().at(index + 1);
            points.push_back(endPoint);
            boundingBox.extend(endPoint);

            pointSections.push_back(std::make_shared<StaticPolyline>(points));
        }
        return std::make_shared<CollisionDetectionRepresentation>(pointSections, boundingBox);
    };

    /**
     * Returns a list of polylines between the basepoints of this component with the specified resolution
     *
     * @param resolution the step size for the parameter t. This is roughly equivalent to the euclidean distance of the
     * points in the resulting poly-object.
     * @return the poly-object
     */
    std::shared_ptr<U> calculatePolyRepresentation(floatType resolution) const {
        std::vector<Vector> points{};
        floatType relativeT = this->startT;
        while (relativeT < this->endT) {
            points.push_back(this->evaluate(relativeT));
            relativeT += resolution;
        }
        points.push_back(this->endPoint);
        return std::make_shared<U>(points);
    };

    /**
     * Evaluates this component at the given relative position.
     *
     * @param relativeT
     * @return Corresponding position.
     */
    virtual Vector evaluate(floatType relativeT) const = 0;

    std::shared_ptr<U> getBasePoints() const { return this->polyObject; }

    AxisAlignedBoundingBox getBoundingBox() { return this->polyObject.getBoundingBox(); };

    ///**
    // * Calculates a List of all intersections of the given line with this poly object
    // *
    // * @param start
    // * @param end
    // * @return
    // */
    // std::vector<std::shared_ptr<Intersection>> calculateIntersections(const Line &line) const {
    //    std::vector<std::shared_ptr<Intersection>> intersections = {};
    //    std::vector<std::shared_ptr<StaticPolyline>> polylineSections =
    //    this->computeRelevantSectionsCollisionDetection(line); for (const StaticPolyline &section : polylineSections)
    //    {
    //        std::vector<Intersection> newIntersections = section.calculateIntersections(line);
    //        intersections.insert(intersections.end(), newIntersections.cbegin(), newIntersections.cend());
    //    }
    //    return intersections;
    //}

  protected:
    std::shared_ptr<U> polyObject;
    std::vector<floatType> relativeTs;
    TerrainConstants terrainConstants;
    Vector startPoint = {};
    Vector endPoint = {};
    floatType startT = 0;
    floatType endT = 0;

  private:
    /**
     * Evaluates this component from the given startT to the given endT with step size resolution.
     *
     * @param sectionStartT
     * @param sectionEndT
     * @param resolution
     * @return
     */
    std::vector<Vector> evaluateInTSection(floatType sectionStartT, floatType sectionEndT, floatType resolution) const {
        std::vector<Vector> points = {};
        floatType relativeT = sectionStartT;
        while (relativeT < sectionEndT) {
            points.push_back(this->evaluate(relativeT));
            relativeT += resolution;
        }
        points.push_back(this->evaluate(sectionEndT));
        return points;
    }

    ///**
    // * Computes the section of this component inside the bounding box with the collision detection resolution.
    // *
    // * @param boundingBox
    // * @return
    // */
    // std::vector<std::shared_ptr<StaticPolyline>>
    // computeRelevantSectionsCollisionDetection(const AxisAlignedBoundingBox &boundingBox) const {
    //    // floatType sectionStartT = this->startT;
    //    // floatType sectionEndT = this->startT;
    //    std::vector<StaticPolyline> sections = {};
    //
    //    for (int index = 0; index < this->getBasePoints()->getPoints().size() - 1; index++) {
    //        floatType sectionStartT = this->relativeTs.at(index);
    //        floatType sectionEndT = this->relativeTs.at(index + 1);
    //        Vector firstPoint = this->getBasePoints()->getPoints().at(index);
    //        Vector secondPoint = this->getBasePoints()->getPoints().at(index + 1);
    //        Line line = {firstPoint, secondPoint};
    //        if (boundingBox.intersectsOrIsIn(line)) {
    //            std::vector<Vector> newPoints = this->evaluateInTSection(
    //                sectionStartT, sectionEndT, this->terrainConstants.collisionDetectionResolution);
    //            sections.emplace_back(newPoints);
    //        }
    //    }
    //    return sections;
    //}

    ///**
    // * Computes the section of this component along the line with the collision detection resolution.
    // *
    // * @param line
    // * @return
    // */
    // std::vector<StaticPolyline> computeRelevantSectionsCollisionDetection(const Line &line) const {
    //    return this->computeRelevantSectionsCollisionDetection(AxisAlignedBoundingBox::getBoundingBoxWithTolerance(
    //        line, this->terrainConstants.collisionDetectionPreFilteringTolerance));
    //}
};

#endif // SURVIVING_SARNTAL_COMPONENT_HPP
