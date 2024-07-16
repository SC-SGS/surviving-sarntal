//
// Created by felix-roehr on 7/6/24.
//

#include "HermiteSpline.hpp"
#include "spdlog/spdlog.h"

const Polynomial HermiteSpline::polynomialXLeft = *(new Polynomial({2, -3, 0, 1}));
const Polynomial HermiteSpline::polynomialXRight = *(new Polynomial({-2, 3, 0, 0}));
const Polynomial HermiteSpline::polynomialLeftDer = *(new Polynomial({1, -2, 1, 0}));
const Polynomial HermiteSpline::polynomialRightDer = *(new Polynomial({1, -1, 0, 0}));

HermiteSpline::HermiteSpline(Vector leftPos, Vector rightPos, floatType leftDer, floatType rightDer) {
    this->xLeft = leftPos.x;
    this->xRight = rightPos.x;
    this->yLeft = leftPos.y;
    this->yRight = rightPos.y;
    this->leftDer = leftDer;
    this->rightDer = rightDer;
}

floatType HermiteSpline::evaluate(floatType xPos) const {
    // floatType width = this->xRight - this->xLeft;
    // floatType relativeT = (xPos - this->xLeft) / width;
    // floatType result = 0;
    // result += polynomialXLeft.evaluate(relativeT) * this->yLeft;
    //// spdlog::info("Result with left pos: {}\n", result);
    // result += polynomialXRight.evaluate(relativeT) * this->yRight;
    //// spdlog::info("Right pos: ({},{})", this->xRight, this->yRight);
    //// spdlog::info("Result with right pos: {}\n", result);
    // result += polynomialLeftDer.evaluate(relativeT) * width * this->leftDer;
    //// spdlog::info("Result with left der: {}\n", result);
    // result += polynomialRightDer.evaluate(relativeT) * width * this->rightDer;
    //// spdlog::info("Result with right der: {}\n", result);
    //
    //// spdlog::info("END RESULT: {}", result);
    return this->evaluateDerivative(xPos, 0);
}

floatType HermiteSpline::evaluateDerivative(floatType xPos) const { return this->evaluateDerivative(xPos, 1); }

floatType HermiteSpline::evaluateDerivative(floatType xPos, int n) const {
    floatType width = this->xRight - this->xLeft;
    floatType relativeT = (xPos - this->xLeft) / width;
    floatType result = 0;
    result += polynomialXLeft.evaluateDerivative(relativeT, n) * this->yLeft;
    // spdlog::info("Result with left pos: {}\n", result);
    result += polynomialXRight.evaluateDerivative(relativeT, n) * this->yRight;
    // spdlog::info("Right pos: ({},{})", this->xRight, this->yRight);
    // spdlog::info("Result with right pos: {}\n", result);
    result += polynomialLeftDer.evaluateDerivative(relativeT, n) * width * this->leftDer;
    // spdlog::info("Result with left der: {}\n", result);
    result += polynomialRightDer.evaluateDerivative(relativeT, n) * width * this->rightDer;
    // spdlog::info("Result with right der: {}\n", result);

    // spdlog::info("END RESULT: {}", result);
    return result;
}

bool HermiteSpline::isInRange(floatType xPos) const { return xPos >= this->xLeft && xPos <= this->xRight; }
