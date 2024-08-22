//
// Created by Anietta Weckauff on 06.08.24.
//

#include "Polygon.h"
#include <cmath>

Polygon::Polygon(std::vector<Vector> &vertices) : vertices(vertices) {}

std::vector<Vector> Polygon::getVertices() { return vertices; }

Rectangle Polygon::getBoundingBox() {
    floatType minX = vertices[0].x, maxX = vertices[0].x;
    floatType minY = vertices[0].y, maxY = vertices[0].y;

    for (const auto &vertex : vertices) {
        if (vertex.x < minX)
            minX = vertex.x;
        if (vertex.x > maxX)
            maxX = vertex.x;
        if (vertex.y < minY)
            minY = vertex.y;
        if (vertex.y > maxY)
            maxY = vertex.y;
    }

    return {minX, minY, maxX - minX, maxY - minY};
}

Vector Polygon::calculateCentroid() {
    floatType signedArea = 0.0f;
    floatType centroidX = 0.0f;
    floatType centroidY = 0.0f;

    size_t numVertices = vertices.size();
    for (size_t i = 0; i < numVertices; ++i) {
        size_t nextIndex = (i + 1) % numVertices;

        floatType xCoordinate1 = vertices[i].x;
        floatType yCoordinate1 = vertices[i].y;
        floatType xCoordinate2 = vertices[nextIndex].x;
        floatType yCoordinate2 = vertices[nextIndex].y;

        floatType crossProduct = xCoordinate1 * yCoordinate2 - xCoordinate2 * yCoordinate1;
        signedArea += crossProduct;
        centroidX += (xCoordinate1 + xCoordinate2) * crossProduct;
        centroidY += (yCoordinate1 + yCoordinate2) * crossProduct;
    }

    signedArea *= 0.5f;
    centroidX /= (6.0f * signedArea);
    centroidY /= (6.0f * signedArea);

    return {centroidX, centroidY};
}
Vector Polygon::calculateCenter() {

    floatType sumXCoordinates = 0;
    floatType sumYCoordinates = 0;

    for (auto &vertice : vertices) {
        sumXCoordinates += vertice.x;
        sumYCoordinates += vertice.y;
    }

    floatType centerX = sumXCoordinates / static_cast<floatType>(vertices.size());
    floatType centerY = sumYCoordinates / static_cast<floatType>(vertices.size());

    std::cout << centerX << std::endl;
    std::cout << centerY << std::endl;

    return {centerX, centerY};
}

void Polygon::move(floatType xOffset, floatType yOffset) {
    for (auto &vertex : vertices) {
        vertex.x += xOffset;
        vertex.y += yOffset;
    }
}

void Polygon::rotate(floatType angularOffset) {
    floatType radians = angularOffset * static_cast<floatType>(M_PI / 180.0f);
    Vector center = this->calculateCentroid();

    floatType cosTheta = std::cos(radians);
    floatType sinTheta = std::sin(radians);

    for (auto &vertex : vertices) {
        // move point relative to center
        float translatedX = vertex.x - center.x;
        float translatedY = vertex.y - center.y;

        // apply rotation matrix
        float rotatedX = translatedX * cosTheta - translatedY * sinTheta;
        float rotatedY = translatedX * sinTheta + translatedY * cosTheta;

        // move point back
        vertex.x = rotatedX + center.x;
        vertex.y = rotatedY + center.y;
    }
}

std::vector<Vector2> Polygon::getRaylibVertices() {
    std::vector<Vector2> raylibVertices;
    raylibVertices.reserve(vertices.size() + 1);
    for (auto vertice : vertices) {
        raylibVertices.push_back({vertice.x, vertice.y});
    }
    raylibVertices.push_back({vertices[0].x, vertices[0].y});
    return raylibVertices;
}

std::vector<Vector2> Polygon::determineTextureCoordinates() {
    std::vector<Vector2> raylibVertices = this->getRaylibVertices();
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

    float width = maxX - minX;
    float height = maxY - minY;

    for (auto &raylibVertice : raylibVertices) {
        float normalizedX = (raylibVertice.x - minX) / width;
        float normalizedY = (raylibVertice.y - minY) / height;
        texCoords.push_back({normalizedX, normalizedY});
    }

    return texCoords;
}

std::vector<Vector2> Polygon::getTextureCoordinates() { return textureCoordinates; }

std::vector<Vector> Polygon::transformIntoCentroidCoordinateSystem() {
    std::vector<Vector> transformedCoordinates;
    transformedCoordinates.reserve(vertices.size());
    Vector centroid = this->calculateCentroid();
    for (const auto &vertice : vertices) {
        transformedCoordinates.push_back({vertice.x - centroid.x, vertice.y - centroid.y});
    }
    return transformedCoordinates;
}
