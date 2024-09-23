//
// Created by felix-roehr on 7/25/24.
//

#ifndef SURVIVING_SARNTAL_SPLINEOID_HPP
#define SURVIVING_SARNTAL_SPLINEOID_HPP

#include "../points/StaticPolygon.hpp"
#include "../splines/TwoDimensionalHermiteSpline.hpp"
#include "ClosedComponent.hpp"

class Splineoid : public ClosedComponent {
  public:
    explicit Splineoid(
        std::shared_ptr<StaticPolygon> polygon,
        const std::vector<Vector> &derivatives,
        TerrainConstants &terrainConstants); // TODO: Use C2-smooth hermite splines with natural border conditions

  private:
    std::unique_ptr<std::vector<TwoDimensionalHermiteSpline>> splines = {};

    Vector evaluate(floatType relativeT) const override;
};

#endif // SURVIVING_SARNTAL_SPLINEOID_HPP
