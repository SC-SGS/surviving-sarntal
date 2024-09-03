//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Mountain.h"
#include "iostream"
#include <mutex>
#include <random>

Mountain::Mountain(MountainConstants mountainConstants) : mountainConstants(mountainConstants) { generateMountain(); }

void Mountain::generateInitialChunk() {
    Vector pos1 = {mountainConstants.start, mountainConstants.initialHeight};
    spdlog::info("Point generated: ({},{})", pos1.x, pos1.y);
    floatType xRight = mountainConstants.start + mountainConstants.chunkWidth;
    floatType yRight =
        this->rand->getRandomRealNumber(mountainConstants.initialHeight + mountainConstants.newPointDeltaMin,
                                        mountainConstants.initialHeight + mountainConstants.newPointDeltaMax);
    spdlog::info("Point generated: ({},{})", xRight, yRight);
    Vector pos2 = {xRight, yRight};

    floatType der = (pos2.y - pos1.y) / (pos2.x - pos1.x);

    auto *firstSplinePiece = new HermiteSpline(pos1.x, pos2.x, pos1.y, pos2.y, der, der);

    this->positions.push_back(pos1);
    this->derivatives.push_back(der);
    this->spline.push_back(firstSplinePiece);
    this->positions.push_back(pos2);
    this->derivatives.push_back(der);
}

void Mountain::generateNewChunk() {
    Vector prevPos = this->positions.back();

    floatType xNew = prevPos.x + mountainConstants.chunkWidth;
    floatType yNew = this->rand->getRandomRealNumber(prevPos.y + mountainConstants.newPointDeltaMin,
                                                     prevPos.y + mountainConstants.newPointDeltaMax);
    Vector pos = {xNew, yNew};
    spdlog::debug("Point generated: ({},{})", xNew, yNew);

    floatType prevDer = this->derivatives.back();

    floatType der = this->rand->getRandomRealNumber(mountainConstants.slope - mountainConstants.randomness,
                                                    mountainConstants.slope + mountainConstants.randomness);

    auto *nextSplinePiece = new HermiteSpline(prevPos.x, pos.x, prevPos.y, pos.y, prevDer, der);

    this->spline.push_back(nextSplinePiece);
    this->positions.push_back(pos);
    this->derivatives.push_back(der);
}

floatType Mountain::calculateYPos(floatType xPos) const { return this->calculateDerivative(xPos, 0); }

floatType Mountain::calculateDerivative(floatType xPos) const { return this->calculateDerivative(xPos, 1); }

floatType Mountain::calculateDerivative(floatType xPos, int n) const {
    if (!this->isInRange(xPos)) {
        spdlog::debug("Pos {} not in range:", xPos);
    }
    assert(this->isInRange(xPos));
    int index = 0;
    while (xPos > this->positions.at(index + 1).x) {
        index++;
    }
    return this->spline.at(index)->evaluateDerivative(xPos, n);
}

void Mountain::deleteLeftChunk() {
    this->positions.erase(this->positions.cbegin());
    this->derivatives.erase(this->derivatives.cbegin());
    this->spline.erase(this->spline.cbegin());
}

floatType Mountain::getLeftBorder() const { return this->positions.front().x; }

floatType Mountain::getRightBorder() const { return this->positions.back().x; }

bool Mountain::isInRange(floatType xPos) const {
    return this->getLeftBorder() <= xPos && this->getRightBorder() >= xPos;
}

void Mountain::generateMountain() {
    this->generateInitialChunk();
    for (int counter = 1; counter < mountainConstants.chunkCount; counter++) {
        this->generateNewChunk();
    }
}

Vector Mountain::calculateDerivativeVector(floatType xPos) const {
    Vector vector = {1.0f, this->calculateDerivative(xPos)};
    return vector.normalize();
}

Vector Mountain::calculateNormalVector(floatType xPos) const {
    Vector derivative = this->calculateDerivativeVector(xPos);
    Vector normal = {-1.0f * derivative.y, derivative.x};
    return normal;
}
