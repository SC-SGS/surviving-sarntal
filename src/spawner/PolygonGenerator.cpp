//
// Created by Anietta Weckauff on 06.08.24.
//

#include "PolygonGenerator.h"

#include "../output/graphics/renderers/PolygonRenderer.h"
#include "../utilities/RandomGenerator.hpp"
#include <algorithm>

PolygonGenerator::PolygonGenerator() = default;

DynamicPolygon PolygonGenerator::generatePolygon(const int numberPoints,
                                                 const floatType maxRadius,
                                                 const Vector &position,
                                                 const floatType density) const {
    std::vector<Vector> points = generatePoints(numberPoints, maxRadius);
    const std::vector<Vector> vertices = determineVertices(points);
    if (!isAnticlockwise(vertices)) {
        const std::vector<Vector> anticlockwiseVertices = reverseOrderToAnticlockWise(vertices);
    }
    const Vector centroid = calculateCentroid(vertices);
    const std::vector<Vector> bodySpaceVertices = transformIntoCentroidCoordinateSystem(centroid, vertices);
    const std::vector<Vector2> textureCoordinates = determineTextureCoordinates(bodySpaceVertices);
    const floatType mass = calculateAreaBodySpace(bodySpaceVertices) * density;
    const floatType momentOfInertia = calculateInertiaRotCentroidBodySpace(bodySpaceVertices, density);
    return {position, bodySpaceVertices, textureCoordinates, mass, density, momentOfInertia, DynamicProperties()};
}

std::vector<Vector> PolygonGenerator::generatePoints(const int numberPoints, const floatType maxRadius) const {
    std::vector<Vector> points(numberPoints);
    for (int i = 0; i < numberPoints; ++i) {
        const floatType angle = randomGenerator->getRandomRealNumber(0, 2 * M_PI);
        const floatType radius = randomGenerator->getRandomRealNumber(0, maxRadius);
        points[i].x = static_cast<floatType>(radius * cos(angle));
        points[i].y = static_cast<floatType>(radius * sin(angle));
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

std::vector<Vector> PolygonGenerator::determineLowerHull(const std::vector<Vector> &points) {
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

std::vector<Vector> PolygonGenerator::determineUpperHull(const std::vector<Vector> &points) {
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

std::vector<Vector> PolygonGenerator::reverseOrderToAnticlockWise(std::vector<Vector> vertices) {
    std::reverse(vertices.begin(), vertices.end());
    return vertices;
}

Vector PolygonGenerator::calculateCentroid(const std::vector<Vector> &vertices) {
    double signedArea = 0.0f;
    double centroidX = 0.0f;
    double centroidY = 0.0f;

    const size_t numVertices = vertices.size();
    for (size_t i = 0; i < numVertices; ++i) {
        const size_t nextIndex = (i + 1) % numVertices;

        const double xCoordinate1 = vertices[i].x;
        const double yCoordinate1 = vertices[i].y;
        const double xCoordinate2 = vertices[nextIndex].x;
        const double yCoordinate2 = vertices[nextIndex].y;

        const double crossProduct = xCoordinate1 * yCoordinate2 - xCoordinate2 * yCoordinate1;
        signedArea += crossProduct;
        centroidX += (xCoordinate1 + xCoordinate2) * crossProduct;
        centroidY += (yCoordinate1 + yCoordinate2) * crossProduct;
    }

    signedArea *= 0.5f;
    centroidX /= (6.0f * signedArea);
    centroidY /= (6.0f * signedArea);

    return {static_cast<floatType>(centroidX), static_cast<floatType>(centroidY)};
}

std::vector<Vector> PolygonGenerator::transformIntoCentroidCoordinateSystem(const Vector &centroid,
                                                                            const std::vector<Vector> &vertices) {
    std::vector<Vector> transformedCoordinates;
    transformedCoordinates.reserve(vertices.size());
    for (const auto &vertex : vertices) {
        transformedCoordinates.push_back({vertex.x - centroid.x, vertex.y - centroid.y});
    }
    return transformedCoordinates;
}

std::vector<Vector2> PolygonGenerator::determineTextureCoordinates(const std::vector<Vector> &vertices) {
    const std::vector<Vector2> raylibVertices = PolygonRenderer::getRaylibVertices(vertices);
    std::vector<Vector2> texCoords;

    float minX = raylibVertices[0].x;
    float maxX = raylibVertices[0].x;
    float minY = raylibVertices[0].y;
    float maxY = raylibVertices[0].y;

    for (const auto &vertex : raylibVertices) {
        if (vertex.x < minX)
            minX = vertex.x;
        if (vertex.x > maxX)
            maxX = vertex.x;
        if (vertex.y < minY)
            minY = vertex.y;
        if (vertex.y > maxY)
            maxY = vertex.y;
    }

    const float width = maxX - minX;
    const float height = maxY - minY;

    for (const auto &raylibVertex : raylibVertices) {
        const float normalizedX = (raylibVertex.x - minX) / width;
        const float normalizedY = (raylibVertex.y - minY) / height;
        texCoords.push_back({normalizedX, normalizedY});
    }

    return texCoords;
}
floatType PolygonGenerator::calculateAreaBodySpace(const std::vector<Vector> &vertices) {
    if (!isAnticlockwise(vertices)) {
        throw std::invalid_argument("Cannot calculate area. Vertex List is not given in anticlockwise order.");
    }
    double area = 0.0f;
    const size_t numVertices = vertices.size();
    for (size_t i = 1; i < numVertices; ++i) {
        area += vertices[i - 1].cross(vertices[i]);
    }
    area += vertices[numVertices - 1].cross(vertices[0]);
    area *= 0.5;
    return static_cast<floatType>(area);
}
floatType PolygonGenerator::calculateInertiaRotCentroidBodySpace(const std::vector<Vector> &vertices,
                                                                 const floatType density) {
    if (!isAnticlockwise(vertices)) {
        throw std::invalid_argument("Cannot calculate inertia. Vertex List is not given in anticlockwise order.");
    }
    const auto bodyCentroid = calculateCentroid(vertices);
    if (!(vertices[0] + bodyCentroid).approxEq(vertices[0], 0.0001)) {
        throw std::invalid_argument(
            "Cannot calculate inertia. Vertex List is not given in centroid coordinate system.");
    }
    double momentOfInertia = 0.0f;
    const size_t numVertices = vertices.size();
    for (size_t i = 0; i < numVertices; ++i) {
        momentOfInertia +=
            calculateInertiaOfTriangleRotatingAroundOriginP0(vertices[i], vertices[(i + 1) % numVertices], density);
    }
    return static_cast<floatType>(momentOfInertia);
}
bool PolygonGenerator::isAnticlockwise(const std::vector<Vector> &vertices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        const Vector &vec1 = vertices[i];
        const Vector &vec2 = vertices[(i + 1) % vertices.size()];
        // const floatType cos = vec1.dot(vec2)/(vec1.length() * vec2.length());
        const floatType sin = vec1.cross(vec2) / (vec1.length() * vec2.length());
        if (sin < 0) {
            return false;
        }
    }
    return true;
}

floatType PolygonGenerator::calculateInertiaOfTriangleRotatingAroundOriginP0(const Vector &vertexA,
                                                                             const Vector &vertexB,
                                                                             const floatType density) {
    if (vertexA.cross(vertexB) < 0) {
        throw std::invalid_argument(
            "Cannot calculate triangle inertia. Vertices are not given in anticlockwise order.");
    }
    const Vector vertexH = vertexA - vertexA.projectOn(vertexB - vertexA);
    const double height = vertexH.length();
    const double width1 = (vertexH - vertexA).length();
    const double width2 = (vertexB - vertexH).length();

    const double sign = vertexH.cross(vertexB) > 0 ? 1 : -1;

    const double momentOfInertia = height * pow(width1, 3) / 4.0 + pow(height, 3) * width1 / 12.0 +
                                   sign * (height * pow(width2, 3) / 4.0 + pow(height, 3) * width2 / 12.0);
    return static_cast<floatType>(density * momentOfInertia);
}
