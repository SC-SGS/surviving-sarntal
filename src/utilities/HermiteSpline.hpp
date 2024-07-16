//
// Created by felix-roehr on 7/6/24.
//

#ifndef SURVIVING_SARNTAL_HERMITESPLINE_HPP
#define SURVIVING_SARNTAL_HERMITESPLINE_HPP

#include "GameConstants.hpp"
#include "Polynomial.hpp"
#include "vector.h"

/**
 * This class represents a cubic hermite spline constructed from two given points a, b and their derivatives da, db
 * using the following formula: h(t) = (1- 3t^2 + 2t^3)a + (3t^2 - 2t^3)b + (t-2t^2 + t^3)(b-a)da + (t^3 - t^2)(b-a)db.
 */
class HermiteSpline {
  public:
    HermiteSpline(Vector leftPos, Vector rightPos, floatType leftDer, floatType rightDer);
    ~HermiteSpline() = default;

    /**
     * Evaluates this spline piece at the given xPos
     *
     * @param xPos
     * @return corresponding yPos
     */
    floatType evaluate(floatType xPos) const;

    /**
     * Evaluates the first evaluateDerivative of this spline piece at the given xPos.
     *
     * @param xPos
     * @return corresponding value of the evaluateDerivative
     */
    floatType evaluateDerivative(floatType xPos) const;

    /**
     * Evaluates the n-th evaluateDerivative of this spline piece at the given xPos.
     *
     * @param xPos
     * @param n
     * @return corresponding value of the evaluateDerivative
     */
    floatType evaluateDerivative(floatType xPos, int n) const;

  private:
    floatType xLeft;
    floatType xRight;
    floatType yLeft;
    floatType yRight;
    floatType leftDer;
    floatType rightDer;

    static const Polynomial polynomialXLeft;
    static const Polynomial polynomialXRight;
    static const Polynomial polynomialLeftDer;
    static const Polynomial polynomialRightDer;

    bool isInRange(floatType xPos) const;
};

#endif // SURVIVING_SARNTAL_HERMITESPLINE_HPP
