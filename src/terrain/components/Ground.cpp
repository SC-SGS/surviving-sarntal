//
// Created by felix-roehr on 7/25/24.
//

#include "Ground.hpp"
#include "../../utilities/ConfigManager.hpp"
#include "../../utilities/Singleton.hpp"
#include <cassert>

Ground::Ground(const std::shared_ptr<StaticPolyline> &polyline,
               const std::vector<Vector> &derivatives,
               floatType startT,
               const TerrainConstants &terrainConstants)
    : Component(polyline, terrainConstants), derivatives(std::make_unique<std::vector<Vector>>(derivatives)) {
    std::vector<Vector> initialPoint = {polyline->getStartPoint()};
    std::vector<Vector> initialDerivative = {derivatives.front()};
    this->derivatives = std::make_shared<std::vector<Vector>>(initialDerivative);
    this->polyObject = std::make_unique<StaticPolyline>(initialPoint);
    this->endPoint = polyline->getStartPoint();
    this->startT = startT;
    this->endT = startT;
    this->relativeTs.push_back(startT);
    this->addTerrain(polyline, derivatives);
}

void Ground::addTerrain(Vector &newPoint, Vector &newDerivative) {
    Vector prevPoint = this->endPoint;
    Vector prevDerivative = this->derivatives->back();
    floatType prevT = this->endT;
    floatType newT = prevT + prevPoint.distanceTo(newPoint);
    auto spline =
        std::make_unique<TwoDimensionalHermiteSpline>(prevT, newT, prevPoint, newPoint, prevDerivative, newDerivative);
    this->splines.push_back(std::move(spline));
    this->derivatives->push_back(newDerivative);
    this->polyObject->addPoint(newPoint);
    this->endT = newT;
    this->relativeTs.push_back(newT);
    this->endPoint = newPoint;
}

void Ground::addTerrain(const std::shared_ptr<StaticPolyline> &polyline, const std::vector<Vector> &derivatives) {
    assert(polyline->getPoints().size() == derivatives.size());
    assert(polyline->getPoints().front() == this->endPoint);

    // floatType currentT = this->endT;
    for (int counter = 1; counter < polyline->getPoints().size(); counter++) {
        // Vector prevPoint = polyline->getPoints().at(counter - 1);
        // Vector prevDerivative = derivatives->at(counter - 1);
        Vector newPoint = polyline->getPoints().at(counter);
        Vector newDerivative = derivatives.at(counter);
        this->addTerrain(newPoint, newDerivative);

        // floatType prevT = currentT;
        // currentT += prevPoint.distanceTo(newPoint);
        // auto spline = TwoDimensionalHermiteSpline(prevT, currentT, prevPoint, newPoint,
        //                                           derivatives->at(counter - 1), derivatives->at(counter));
        // this->splines.push_back(spline);
    }
    // this->endT = currentT;
    // this->endPoint = polyline->getEndPoint();
}

floatType Ground::getEndT() const { return endT; }

// void Ground::removeTerrain(floatType minXPos) {
//     floatType currentXPos = this->startPoint.x;
//     int index = -1;
//     while(currentXPos < minXPos && index < this->splines.size() - 1) {
//         index++;
//         currentXPos = this->splines.at(index).getLeftPos().x;
//     }
//
//     this->splines.erase(this->splines.cbegin(), this->splines.cbegin() + index + 1);
//     this->startT = this->splines.front().getLeftT();
//     this->startPoint = this->splines.front().getLeftPos();
// }

std::shared_ptr<std::vector<Vector>> Ground::getDerivatives() const { return this->derivatives; }

Vector Ground::evaluate(floatType relativeT) const {
    assert(this->startT <= relativeT && relativeT <= this->endT);
    int index = -1;
    floatType currentT;
    do {
        index++;
        currentT = this->splines.at(index)->getRightT();
    } while (relativeT > currentT);
    return this->splines.at(index)->evaluate(relativeT);
}

floatType Ground::getMinT(floatType minX) {
    assert(this->getBasePoints()->getStartPoint().x < minX && minX < this->getBasePoints()->getEndPoint().x);
    int index = 0;
    while (this->splines.at(index)->getRightPos().x < minX) {
        index++;
    }
    return this->splines.at(index)->getLeftT();
}

floatType Ground::getMaxT(floatType maxX) {
    assert(this->getBasePoints()->getStartPoint().x < maxX && maxX < this->getBasePoints()->getEndPoint().x);
    auto index = static_cast<int>(this->getBasePoints()->getPoints().size() - 1);
    while (this->splines.at(index)->getLeftPos().x > maxX) {
        index--;
    }
    return this->splines.at(index)->getRightT();
}

std::shared_ptr<StaticPolyline>
Ground::getPolyRepresentationLastPointToNewPoint(Vector &newPoint, Vector &newDerivative, floatType resolution) const {
    floatType currentT = this->endT + this->endPoint.distanceTo(newPoint);
    auto spline = TwoDimensionalHermiteSpline(this->endT, currentT, this->endPoint, newPoint, this->derivatives->back(),
                                              newDerivative);
    return spline.getPolyrepresentation(resolution);
}

std::shared_ptr<StaticPolyline> Ground::getPolyRepresentationForGeneration() const {
    int index = 0;
    while (index < this->getBasePoints()->getPoints().size() &&
           this->getBasePoints()->getPoints().at(index).distanceTo(this->endPoint) >
               this->terrainConstants.generationCheckingRange) {
        index++;
    }
    if (index > 0) {
        index--;
    }

    std::vector<Vector> points = {};
    floatType relativeT = this->splines.at(index)->getLeftT();
    while (relativeT < this->endT) {
        points.push_back(this->evaluate(relativeT));
        relativeT += this->terrainConstants.renderingResolution;
    }
    points.push_back(this->endPoint);
    return std::make_shared<StaticPolyline>(points);
}

void Ground::removeLastBasepoints(int count) {
    assert(this->splines.size() >= count);
    for (int counter = 0; counter < count; counter++) {
        this->splines.pop_back();
        this->derivatives->pop_back();
        this->relativeTs.pop_back();
    }
    this->polyObject->removeLastPoints(count);
    this->endPoint = this->polyObject->getEndPoint();
    this->endT = this->splines.back()->getRightT();
}