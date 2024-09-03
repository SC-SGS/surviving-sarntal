//
// Created by felix-roehr on 7/6/24.
//

#ifndef SURVIVING_SARNTAL_HERMITESPLINE_HPP
#define SURVIVING_SARNTAL_HERMITESPLINE_HPP

#include "../../game/GameProperties.hpp"
#include "../../utilities/vector.h"
#include "Polynomial.hpp"
#include "Spline.hpp"

/**
 * This class represents a cubic hermite spline constructed from two given points a, b and their derivatives da, db
 * using the following formula: h(t) = (1- 3t^2 + 2t^3)a + (3t^2 - 2t^3)b + (t-2t^2 + t^3)(b-a)da + (t^3 - t^2)(b-a)db.
 */
class HermiteSpline : public Spline<floatType> {
  public:
    HermiteSpline(floatType leftT, floatType rightT, floatType leftValue, floatType rightValue, floatType leftDer,
                  floatType rightDer);
    ~HermiteSpline() override = default;

    floatType evaluateDerivative(floatType tPos, int n) const override;

  private:
    floatType leftValue;
    floatType rightValue;
    floatType leftDer;
    floatType rightDer;

    static const Polynomial polynomialLeftValue;
    static const Polynomial polynomialRightValue;
    static const Polynomial polynomialLeftDer;
    static const Polynomial polynomialRightDer;
};

#endif // SURVIVING_SARNTAL_HERMITESPLINE_HPP
