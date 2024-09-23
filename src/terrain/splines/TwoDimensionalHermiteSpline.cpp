//
// Created by felix-roehr on 7/26/24.
//

#include "TwoDimensionalHermiteSpline.hpp"
#include "HermiteSpline.hpp"

TwoDimensionalHermiteSpline::TwoDimensionalHermiteSpline(
    floatType startT, floatType endT, Vector start, Vector end, Vector startDer, Vector endDer)
    : Spline(startT, endT),
      start(start),
      end(end),
      startDer(startDer.normalize()),
      endDer(endDer.normalize()),
      startT(startT),
      endT(endT) {
    this->xSpline = std::make_unique<HermiteSpline>(startT, endT, start.x, end.x, this->startDer.x, this->endDer.x);
    this->ySpline = std::make_unique<HermiteSpline>(startT, endT, start.y, end.y, this->startDer.y, this->endDer.y);
}

Vector TwoDimensionalHermiteSpline::evaluateDerivative(floatType tPos, int n) const {
    floatType xVal = this->xSpline->evaluateDerivative(tPos, n);
    floatType yVal = this->ySpline->evaluateDerivative(tPos, n);
    return {xVal, yVal};
}

std::shared_ptr<StaticPolyline> TwoDimensionalHermiteSpline::getPolyrepresentation(floatType resolution) const {
    std::vector<Vector> points = {};
    floatType relativeT = this->startT;
    while (relativeT <= this->endT) {
        points.push_back(this->evaluate(relativeT));
        relativeT += resolution;
    }
    if (relativeT < this->endT) {
        points.push_back(this->end);
    }
    return std::make_shared<StaticPolyline>(points);
}
