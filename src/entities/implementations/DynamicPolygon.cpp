//
// Created by Anietta Weckauff on 06.08.24.
//

#include "../DynamicPolygon.h"

#include "../../spawner/PolygonGenerator.h"

#include <cmath>
#include <iostream>
#include <ostream>

DynamicPolygon::DynamicPolygon(const Vector &position, const std::vector<Vector> &vertices,
                               const std::vector<Vector2> &textureCoordinates, const floatType mass, floatType density,
                               const floatType momentOfInertia, const DynamicProperties &dynamicProperties)
    : RenderedEntity(position), dynamicProperties(dynamicProperties), bodySpaceVertices(vertices),
      textureCoordinates(textureCoordinates), mass(mass), density(density), momentOfInertia(momentOfInertia) {
    if (vertices.size() + 1 != textureCoordinates.size()) {
        throw std::invalid_argument(
            "Dynamic Polygon cannot be generated. Vertex List and TexCoords list do not have sizes n and n+1.");
    }
    if (textureCoordinates[0].x != textureCoordinates[textureCoordinates.size() - 1].x ||
        textureCoordinates[0].y != textureCoordinates[textureCoordinates.size() - 1].y) {
        throw std::invalid_argument("Dynamic Polygon cannot be generated. Texture coordinates are not cyclic.");
    }
    const auto bodySpaceCentroid = PolygonGenerator::calculateCentroid(this->bodySpaceVertices);
    // here position is added for stability
    if (!(bodySpaceCentroid + this->position).approxEq(this->position, 0.0001)) {
        throw std::invalid_argument(
            "Dynamic Polygon cannot be generated. Vertex List is not given in centroid coordinate system.");
    }
    if (!PolygonGenerator::isAnticlockwise(this->bodySpaceVertices)) {
        throw std::invalid_argument(
            "Dynamic Polygon cannot be generated. Vertex List is not given in anticlockwise order.");
    }
}

DynamicPolygon::DynamicPolygon(const Vector &position, const std::vector<Vector> &vertices, floatType mass)
    : RenderedEntity(position), bodySpaceVertices(vertices), mass(mass),
      density(this->calculateDensity(vertices, mass)), momentOfInertia(this->calculateMomentOfInertia(vertices, mass)) {
}

floatType DynamicPolygon::getRotationAngle() const { return this->dynamicProperties.rotationAngleRad; }
floatType DynamicPolygon::getMass() const { return this->mass; }
floatType DynamicPolygon::getMomentOfInertia() const { return this->momentOfInertia; }

const std::vector<Vector> &DynamicPolygon::getBodySpaceVertices() const { return this->bodySpaceVertices; }

std::vector<Vector> DynamicPolygon::getWorldSpaceVertices() const {
    const floatType cosTheta = std::cos(this->dynamicProperties.rotationAngleRad);
    const floatType sinTheta = std::sin(this->dynamicProperties.rotationAngleRad);

    std::vector<Vector> worldCoordinates;
    worldCoordinates.reserve(this->bodySpaceVertices.size());
    std::copy(this->bodySpaceVertices.begin(), this->bodySpaceVertices.end(), std::back_inserter(worldCoordinates));

    for (auto &vertex : worldCoordinates) {
        // rotation
        const float rotatedX = vertex.x * cosTheta - vertex.y * sinTheta;
        const float rotatedY = vertex.x * sinTheta + vertex.y * cosTheta;

        // translation
        vertex.x = rotatedX + this->position.x;
        vertex.y = rotatedY + this->position.y;
    }
    return worldCoordinates;
}

Rectangle DynamicPolygon::getBoundingBox() const {
    floatType minX = bodySpaceVertices[0].x, maxX = bodySpaceVertices[0].x;
    floatType minY = bodySpaceVertices[0].y, maxY = bodySpaceVertices[0].y;

    for (const auto &vertex : bodySpaceVertices) {
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

void DynamicPolygon::move(const floatType xOffset, const floatType yOffset) {
    this->dynamicProperties.lastPosition = this->position;
    this->position.x += xOffset;
    this->position.y += yOffset;
}
void DynamicPolygon::move(const Vector &offset) {
    this->dynamicProperties.lastPosition = this->position;
    this->position += offset;
}

void DynamicPolygon::rotate(const floatType angularOffset) {
    this->dynamicProperties.rotationAngleRad += angularOffset;
}

const std::vector<Vector2> &DynamicPolygon::getTextureCoordinates() const { return textureCoordinates; }

RenderInformation DynamicPolygon::getRenderInformation() const {
    return RenderInformation{Vector2(position),
                             {0, 0},
                             0.f,
                             0.f,
                             this->dynamicProperties.rotationAngleRad * static_cast<floatType>(180.0f / M_PI),
                             "default"};
}

void DynamicPolygon::setLinearMomentum(const Vector &newLinearMomentum) {
    this->dynamicProperties.linearMomentum = newLinearMomentum;
}

const Vector &DynamicPolygon::getLinearMomentum() const { return this->dynamicProperties.linearMomentum; }

void DynamicPolygon::setAngularMomentum(const floatType newAngularMomentum) {
    this->dynamicProperties.angularMomentum = newAngularMomentum;
}
void DynamicPolygon::setRotationAngleRad(const floatType newRotationAngleRad) {
    this->dynamicProperties.rotationAngleRad = newRotationAngleRad;
}
floatType DynamicPolygon::getAngularMomentum() const { return this->dynamicProperties.angularMomentum; }
floatType DynamicPolygon::getRotationAngleRad() const { return this->dynamicProperties.rotationAngleRad; }
const DynamicProperties &DynamicPolygon::getDynamicProperties() const { return this->dynamicProperties; }
floatType DynamicPolygon::getDensity() const { return this->density; }

floatType DynamicPolygon::calculateDensity(const std::vector<Vector> &vertices, const floatType mass) const {
    return mass / PolygonGenerator::calculateAreaBodySpace(vertices);
}

floatType DynamicPolygon::calculateMomentOfInertia(const std::vector<Vector> &vertices, const floatType mass) const {
    floatType densityTmp = this->calculateDensity(vertices, mass);
    return PolygonGenerator::calculateInertiaRotCentroidBodySpace(vertices, densityTmp);
}
