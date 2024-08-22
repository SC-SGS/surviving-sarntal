//
// Created by Anietta Weckauff on 06.08.24.
//

#include "PolygonGenerator.h"
#include "../../utilities/RandomGenerator.hpp"
#include <algorithm>

PolygonGenerator::PolygonGenerator() = default;

Polygon PolygonGenerator::generatePolygon(int numberPoints, floatType maxRadius, Vector &center) {
    std::vector<Vector> points = generatePoints(numberPoints, maxRadius, center);
    std::vector<Vector> vertices = determineVertices(points);
    std::vector<Vector> anticlockwiseVertices = this->reverseOrderToAnticklockWise(vertices);
    return Polygon(anticlockwiseVertices);
}

std::vector<Vector> PolygonGenerator::generatePoints(int numberPoints, floatType maxRadius, Vector &center) {
    std::vector<Vector> points(numberPoints);
    for (int i = 0; i < numberPoints; ++i) {
        floatType angle = randomGenerator->getRandomRealNumber(0, 2 * M_PI);
        floatType radius = randomGenerator->getRandomRealNumber(0, maxRadius);
        points[i].x = static_cast<floatType>(center.x + radius * cos(angle));
        points[i].y = static_cast<floatType>(center.y + radius * sin(angle));
    }
    return points;
}

std::vector<Vector> PolygonGenerator::determineVertices(std::vector<Vector> &points) {
    std::vector<Vector> vertices;

    // sort points based on their x coordinate

    std::sort(points.begin(), points.end());
    std::vector<Vector> lowerHull = determineLowerHull(points);
    std::vector<Vector> upperHull = determineUpperHull(points);

    // remove last elements of hull, because it's the same as the first element in the other hull
    lowerHull.pop_back();
    upperHull.pop_back();

    vertices.insert(vertices.end(), lowerHull.begin(), lowerHull.end());
    vertices.insert(vertices.end(), upperHull.begin(), upperHull.end());

    return vertices;
}

std::vector<Vector> PolygonGenerator::determineLowerHull(std::vector<Vector> &points) {
    std::vector<Vector> lowerHull;
    // if the current point and the last two points of the lower hull make a clockwise turn, the last point of the lower
    // hull has to be removed
    for (const auto &vector : points) {
        while (lowerHull.size() >= 2 &&
               calculateCrossProduct(lowerHull[lowerHull.size() - 2], lowerHull[lowerHull.size() - 1], vector) <= 0) {
            lowerHull.pop_back();
        }
        lowerHull.push_back(vector);
    }
    return lowerHull;
}

std::vector<Vector> PolygonGenerator::determineUpperHull(std::vector<Vector> &points) {
    std::vector<Vector> upperHull;
    // if the current point and the last two points of the upper hull make a counter clockwise turn,
    // the last point of the upper hull has to be removed
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        while (upperHull.size() >= 2 &&
               calculateCrossProduct(upperHull[upperHull.size() - 2], upperHull[upperHull.size() - 1], *it) <= 0) {
            upperHull.pop_back();
        }
        upperHull.push_back(*it);
    }
    return upperHull;
}

floatType PolygonGenerator::calculateCrossProduct(const Vector &vector0, const Vector &vectorA, const Vector &vectorB) {
    return ((vectorA.x - vector0.x) * (vectorB.y - vector0.y)) - ((vectorA.y - vector0.y) * (vectorB.x - vector0.x));
}

std::vector<Vector> PolygonGenerator::reverseOrderToAnticklockWise(std::vector<Vector> &vertices) {
    std::reverse(vertices.begin(), vertices.end());
    return vertices;
}
