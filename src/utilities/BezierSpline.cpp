//
// Created by felix-roehr on 7/7/24.
//

#include "BezierSpline.hpp"

BezierSpline::BezierSpline(floatType pos0, floatType pos1, floatType pos2, floatType pos3)
    : pos0(pos0), pos1(pos1), pos2(pos2), pos3(pos3) {
    this->polynomial = new Polynomial({-this->pos0 + 3 * this->pos1 - 3 * this->pos2 + this->pos3,
                                       3 * this->pos0 - 6 * this->pos1 + 3 * this->pos2,
                                       -3 * this->pos0 + 3 * this->pos1, this->pos0});
}

floatType BezierSpline::getPos0() const { return pos0; }
floatType BezierSpline::getPos1() const { return pos1; }
floatType BezierSpline::getPos2() const { return pos2; }
floatType BezierSpline::getPos3() const { return pos3; }

floatType BezierSpline::evaluate(floatType relativeT) const { return this->polynomial->evaluate(relativeT); }

floatType BezierSpline::evaluateAndTransform(floatType posX, floatType minX, floatType maxX) const {
    floatType relativeT = (posX - minX) / (maxX - minX);
    return this->evaluate(relativeT);
}
