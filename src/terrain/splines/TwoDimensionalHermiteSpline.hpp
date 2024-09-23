//
// Created by felix-roehr on 7/26/24.
//

#ifndef SURVIVING_SARNTAL_TWODIMENSIONALHERMITESPLINE_HPP
#define SURVIVING_SARNTAL_TWODIMENSIONALHERMITESPLINE_HPP

#include "../../utilities/vector.h"
#include "../points/StaticPolyline.hpp"
#include "HermiteSpline.hpp"
#include "Spline.hpp"
#include <type_traits>

class TwoDimensionalHermiteSpline : public Spline<Vector> {
  public:
    TwoDimensionalHermiteSpline(
        floatType startT, floatType endT, Vector start, Vector end, Vector startDer, Vector endDer);
    ~TwoDimensionalHermiteSpline() override = default;

    Vector evaluateDerivative(floatType tPos, int n) const override;

    /**
     * Computes a polyline representation of the spline by evaluating at points spaced by the resolution.
     *
     * @param resolution
     * @return
     */
    std::shared_ptr<StaticPolyline> getPolyrepresentation(floatType resolution) const;

  private:
    std::unique_ptr<HermiteSpline> xSpline;
    std::unique_ptr<HermiteSpline> ySpline;
    Vector start;
    Vector end;
    Vector startDer;
    Vector endDer;
    floatType startT;
    floatType endT;
};

#endif // SURVIVING_SARNTAL_TWODIMENSIONALHERMITESPLINE_HPP
