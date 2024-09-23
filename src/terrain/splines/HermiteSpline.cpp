//
// Created by felix-roehr on 7/6/24.
//

#include "HermiteSpline.hpp"
#include "spdlog/spdlog.h"

const Polynomial HermiteSpline::polynomialLeftValue = Polynomial({2, -3, 0, 1});
const Polynomial HermiteSpline::polynomialRightValue = Polynomial({-2, 3, 0, 0});
const Polynomial HermiteSpline::polynomialLeftDer = Polynomial({1, -2, 1, 0});
const Polynomial HermiteSpline::polynomialRightDer = Polynomial({1, -1, 0, 0});

HermiteSpline::HermiteSpline(
    floatType leftT, floatType rightT, floatType leftValue, floatType rightValue, floatType leftDer, floatType rightDer)
    : Spline<floatType>(leftT, rightT),
      leftValue(leftValue),
      rightValue(rightValue),
      leftDer(leftDer),
      rightDer(rightDer) {}

floatType HermiteSpline::evaluateDerivative(floatType tPos, int n) const {
    assert(this->isInRange(tPos) == true);

    floatType width = this->rightT - this->leftT;
    floatType relativeT = (tPos - this->leftT) / width;
    floatType result = 0;
    result += polynomialLeftValue.evaluateDerivative(relativeT, n) * this->leftValue;
    // spdlog::info("Result with left pos: {}\n", result);
    result += polynomialRightValue.evaluateDerivative(relativeT, n) * this->rightValue;
    // spdlog::info("Right pos: ({},{})", this->xRight, this->yRight);
    // spdlog::info("Result with right pos: {}\n", result);
    result += polynomialLeftDer.evaluateDerivative(relativeT, n) * width * this->leftDer;
    // spdlog::info("Result with left der: {}\n", result);
    result += polynomialRightDer.evaluateDerivative(relativeT, n) * width * this->rightDer;
    // spdlog::info("Result with right der: {}\n", result);

    // spdlog::info("END RESULT: {}", result);
    return result;
}