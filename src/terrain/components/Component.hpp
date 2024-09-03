//
// Created by felix-roehr on 8/5/24.
//

#ifndef SURVIVING_SARNTAL_COMPONENT_HPP
#define SURVIVING_SARNTAL_COMPONENT_HPP

#include "../points/StaticPolyObject.hpp"
template <typename U, typename = std::enable_if_t<std::is_base_of_v<StaticPolyObject, U>>> class Component {
  public:
    explicit Component(U *polyObject, TerrainConstants terrainConstants)
        : polyObject(*polyObject), startPoint(polyObject->getPoints().front()),
          endPoint(polyObject->getPoints().back()), terrainConstants(terrainConstants){};

    /**
     * Returns a poly-representation of this component with the specified resolution
     *
     * @param resolution the step size for the parameter t. This is roughly equivalent to the euclidean distance of the
     * points in the resulting poly-object.
     * @return the poly-object
     */
    U *calculatePolyRepresentation(floatType resolution) const {
        std::vector<Vector> points = {};
        floatType relativeT = this->startT;
        while (relativeT < this->endT) {
            points.push_back(this->evaluate(relativeT));
            relativeT += resolution;
        }
        points.push_back(this->endPoint);
        return new U(points);
    };

    /**
     * Evaluates this component at the given relative position.
     *
     * @param relativeT
     * @return Corresponding position.
     */
    virtual Vector evaluate(floatType relativeT) const = 0;

    const U *getBasePoints() const { return &this->polyObject; }

    AxisAlignedBoundingBox getBoundingBox() { return this->polyObject.getBoundingBox(); };

  protected:
    U polyObject;
    TerrainConstants terrainConstants;
    Vector startPoint = {};
    Vector endPoint = {};
    floatType startT = 0;
    floatType endT = 0;
};

#endif // SURVIVING_SARNTAL_COMPONENT_HPP
