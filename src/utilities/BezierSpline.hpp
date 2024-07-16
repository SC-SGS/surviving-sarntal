//
// Created by felix-roehr on 7/7/24.
//

#ifndef SURVIVING_SARNTAL_BEZIERSPLINE_HPP
#define SURVIVING_SARNTAL_BEZIERSPLINE_HPP

#include "GameConstants.hpp"
#include "Polynomial.hpp"

class BezierSpline {
  public:
    BezierSpline(floatType pos0, floatType pos1, floatType pos2, floatType pos3);
    ~BezierSpline() = default;

    floatType evaluateAndTransform(floatType posX, floatType minX, floatType maxX) const;
    floatType getPos0() const;
    floatType getPos1() const;
    floatType getPos2() const;
    floatType getPos3() const;

  private:
    floatType pos0;
    floatType pos1;
    floatType pos2;
    floatType pos3;

    Polynomial *polynomial;

    floatType evaluate(floatType relativeT) const;
};

#endif // SURVIVING_SARNTAL_BEZIERSPLINE_HPP
