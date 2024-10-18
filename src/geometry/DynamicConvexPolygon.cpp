//
// Created by Anietta Weckauff on 06.08.24.
//

#include "DynamicConvexPolygon.h"
#include "../physics/physicists/CollisionDetector.hpp"
#include "../spawner/PolygonGenerator.h"

#include <cassert>
#include <cmath>

size_t DynamicConvexPolygon::polyIDCount = 0;

DynamicConvexPolygon::DynamicConvexPolygon(const Vector &position,
                                           const std::vector<Vector> &vertices,
                                           const std::vector<Vector2> &textureCoordinates,
                                           const floatType mass,
                                           const floatType density,
                                           const floatType momentOfInertia,
                                           const DynamicProperties &dynamicProperties)
    : RenderedEntity(position),
      dynamicProperties(dynamicProperties),
      polyID(polyIDCount),
      bodySpaceVertices(vertices),
      textureCoordinates(textureCoordinates),
      mass(mass),
      density(density),
      momentOfInertia(momentOfInertia) {
    this->updateAfterMovementOrRotation();
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
    if (!(bodySpaceCentroid + this->position).approxEq(this->position, NUMERIC_EPSILON)) {
        throw std::invalid_argument(
            "Dynamic Polygon cannot be generated. Vertex List is not given in centroid coordinate system.");
    }
    if (!PolygonGenerator::isAnticlockwise(this->bodySpaceVertices)) {
        throw std::invalid_argument(
            "Dynamic Polygon cannot be generated. Vertex List is not given in anticlockwise order.");
    }
    polyIDCount++;
}

floatType DynamicConvexPolygon::getRotationAngle() const { return this->dynamicProperties.rotationAngleRad; }
floatType DynamicConvexPolygon::getMass() const { return this->mass; }
floatType DynamicConvexPolygon::getDensity() const { return this->density; }
floatType DynamicConvexPolygon::getMomentOfInertia() const { return this->momentOfInertia; }
size_t DynamicConvexPolygon::getPolyID() const { return this->polyID; }
void DynamicConvexPolygon::setPosition(const Vector &position) {
    this->position = position;
    this->updateAfterMovementOrRotation();
}

Vector DynamicConvexPolygon::getVelocityAtPointInWorldSpace(const Vector &point) const {
    return getVelocityAtPointInBodySpace(point - this->position);
}
Vector DynamicConvexPolygon::getVelocityAtPointInBodySpace(const Vector &point) const {
    const Vector linearVelocity = this->dynamicProperties.linearMomentum / this->mass;
    const Vector angularVelocity =
        point.preCrossZScalar(this->dynamicProperties.angularMomentum / this->momentOfInertia);
    return linearVelocity + angularVelocity;
}
Vector DynamicConvexPolygon::getVelocityAtVertex(const size_t vertexIdx) const {
    assert(vertexIdx < this->bodySpaceVertices.size());
    return this->getVelocityAtPointInBodySpace(this->bodySpaceVertices[vertexIdx]);
}

bool DynamicConvexPolygon::operator==(const DynamicConvexPolygon &other) const {
    return this->polyID == other.getPolyID();
}
bool DynamicConvexPolygon::operator!=(const DynamicConvexPolygon &other) const {
    return this->polyID != other.getPolyID();
}

const std::vector<Vector> &DynamicConvexPolygon::getBodySpaceVertices() const { return this->bodySpaceVertices; }
const std::vector<Vector> &DynamicConvexPolygon::getWorldSpaceVertices() const { return this->worldSpaceVertices; }

void DynamicConvexPolygon::recalculateWorldSpaceVertices() {
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
    this->worldSpaceVertices = worldCoordinates;
}

std::vector<Vector>
DynamicConvexPolygon::interpolateWorldSpaceVerticesBetweenLastAndCurrent(const floatType alpha) const {
    assert(alpha >= -NUMERIC_EPSILON && alpha <= 1 + NUMERIC_EPSILON);
    if (fabsf(1 - alpha) < NUMERIC_EPSILON)
        return this->worldSpaceVertices;
    const floatType rot = this->getInterpolatedRotationAngleRad(alpha);
    const floatType cosTheta = std::cos(rot);
    const floatType sinTheta = std::sin(rot);
    const Vector pos = this->getInterpolatedPosition(alpha);

    std::vector<Vector> worldCoordinates;
    worldCoordinates.reserve(this->bodySpaceVertices.size());
    std::copy(this->bodySpaceVertices.begin(), this->bodySpaceVertices.end(), std::back_inserter(worldCoordinates));

    for (auto &vertex : worldCoordinates) {
        // rotation
        const float rotatedX = vertex.x * cosTheta - vertex.y * sinTheta;
        const float rotatedY = vertex.x * sinTheta + vertex.y * cosTheta;

        // translation
        vertex.x = rotatedX + pos.x;
        vertex.y = rotatedY + pos.y;
    }
    return worldCoordinates;
}

const AABB &DynamicConvexPolygon::getBoundingBox() const { return this->currentAABB; }

AABB DynamicConvexPolygon::getInterpolatedBoundingBox(const floatType alpha) const {
    if (fabsf(1 - alpha) < NUMERIC_EPSILON)
        return this->currentAABB;
    std::vector<Vector> worldSpaceCoordinates = this->interpolateWorldSpaceVerticesBetweenLastAndCurrent(alpha);
    floatType minX = worldSpaceCoordinates[0].x, maxX = worldSpaceCoordinates[0].x;
    floatType minY = worldSpaceCoordinates[0].y, maxY = worldSpaceCoordinates[0].y;

    for (const auto &vertex : worldSpaceCoordinates) {
        if (vertex.x < minX)
            minX = vertex.x;
        if (vertex.x > maxX)
            maxX = vertex.x;
        if (vertex.y < minY)
            minY = vertex.y;
        if (vertex.y > maxY)
            maxY = vertex.y;
    }

    return {{minX, minY}, {maxX, maxY}};
}
const AABB &DynamicConvexPolygon::getSweptBoundingBox() const { return this->sweptAABB; }

void DynamicConvexPolygon::recalculateSweptBoundingBox() {
    const AABB &newBoundingBox = this->currentAABB;
    const AABB oldBoundingBox = this->getInterpolatedBoundingBox(0.0f);
    const floatType minX = std::min(newBoundingBox.getBottomLeft().x, oldBoundingBox.getBottomLeft().x);
    const floatType minY = std::min(newBoundingBox.getBottomLeft().y, oldBoundingBox.getBottomLeft().y);
    const floatType maxX = std::max(newBoundingBox.getTopRight().x, oldBoundingBox.getTopRight().x);
    const floatType maxY = std::max(newBoundingBox.getTopRight().y, oldBoundingBox.getTopRight().y);
    this->sweptAABB = {{minX, minY}, {maxX, maxY}};
}

void DynamicConvexPolygon::recalculateCurrentBoundingBox() {
    floatType minX = this->worldSpaceVertices[0].x, maxX = this->worldSpaceVertices[0].x;
    floatType minY = this->worldSpaceVertices[0].y, maxY = this->worldSpaceVertices[0].y;

    for (const auto &vertex : this->worldSpaceVertices) {
        if (vertex.x < minX) {
            minX = vertex.x;
        }
        if (vertex.x > maxX) {
            maxX = vertex.x;
        }
        if (vertex.y < minY) {
            minY = vertex.y;
        }
        if (vertex.y > maxY) {
            maxY = vertex.y;
        }
    }

    this->currentAABB = {{minX, minY}, {maxX, maxY}};
}

void DynamicConvexPolygon::move(const Vector &offset) {
    this->dynamicProperties.lastPosition = this->position;
    this->position += offset;
    this->updateAfterMovementOrRotation();
}

void DynamicConvexPolygon::rotate(const floatType angularOffset) {
    this->dynamicProperties.lastRotationAngleRad = this->dynamicProperties.rotationAngleRad;
    /* TODO mod 2Pi is already here, but needs to be considered during interpolation
    floatType newRotationAngleRad = this->dynamicProperties.rotationAngleRad + angularOffset;
    if(newRotationAngleRad > 2*M_PIf)
        newRotationAngleRad -= std::floor(newRotationAngleRad/(2*M_PIf)) * 2 * M_PIf;
    this->dynamicProperties.rotationAngleRad = newRotationAngleRad;*/
    this->dynamicProperties.rotationAngleRad += angularOffset;
    this->updateAfterMovementOrRotation();
}

const std::vector<Vector2> &DynamicConvexPolygon::getTextureCoordinates() const { return textureCoordinates; }

RenderInformation DynamicConvexPolygon::getRenderInformation() const {
    return RenderInformation{Vector2(position),
                             {0, 0},
                             0.f,
                             0.f,
                             this->dynamicProperties.rotationAngleRad * static_cast<floatType>(180.0f / M_PI),
                             "default"};
}

void DynamicConvexPolygon::setLinearMomentum(const Vector &newLinearMomentum) {
    this->dynamicProperties.linearMomentum = newLinearMomentum;
}

const Vector &DynamicConvexPolygon::getLinearMomentum() const { return this->dynamicProperties.linearMomentum; }

void DynamicConvexPolygon::setAngularMomentum(const floatType newAngularMomentum) {
    this->dynamicProperties.angularMomentum = newAngularMomentum;
}
void DynamicConvexPolygon::setRotationAngleRad(const floatType newRotationAngleRad) {
    this->dynamicProperties.rotationAngleRad = newRotationAngleRad;
    this->updateAfterMovementOrRotation();
}
void DynamicConvexPolygon::setInterpolatedMovementState(const floatType alpha) {
    assert(alpha >= -NUMERIC_EPSILON && alpha <= 1 + NUMERIC_EPSILON);
    if (fabsf(1 - alpha) < NUMERIC_EPSILON) {
        return;
    }
    const Vector newPosition = this->getInterpolatedPosition(alpha);
    const floatType newRotationAngleRad = this->getInterpolatedRotationAngleRad(alpha);
    this->position = newPosition;
    this->dynamicProperties.rotationAngleRad = newRotationAngleRad;
    this->updateAfterMovementOrRotation();
}

floatType DynamicConvexPolygon::getInterpolatedRotationAngleRad(const floatType alpha) const {
    assert(alpha >= -NUMERIC_EPSILON && alpha <= 1 + NUMERIC_EPSILON);
    return this->dynamicProperties.lastRotationAngleRad +
           alpha * (this->dynamicProperties.rotationAngleRad - this->dynamicProperties.lastRotationAngleRad);
}

Vector DynamicConvexPolygon::getInterpolatedPosition(const floatType alpha) const {
    return this->dynamicProperties.lastPosition + (this->position - this->dynamicProperties.lastPosition) * alpha;
}

floatType DynamicConvexPolygon::getAngularMomentum() const { return this->dynamicProperties.angularMomentum; }
floatType DynamicConvexPolygon::getRotationAngleRad() const { return this->dynamicProperties.rotationAngleRad; }
const DynamicProperties &DynamicConvexPolygon::getDynamicProperties() const { return this->dynamicProperties; }

void DynamicConvexPolygon::updateLastWitness(const size_t otherRockID, const size_t witnessEdgeIdx) {
    assert(witnessEdgeIdx < this->bodySpaceVertices.size());
    this->collisionData.lastWitness.insert_or_assign(otherRockID, witnessEdgeIdx);
}
size_t DynamicConvexPolygon::getLastWitnessTo(const size_t otherRockID) const {
    if (this->collisionData.lastWitness.count(otherRockID)) {
        return this->collisionData.lastWitness.at(otherRockID);
    }
    return 0;
}

void DynamicConvexPolygon::removeWitnessInformationFor(const size_t otherID) {
    if (this->collisionData.lastWitness.count(otherID)) {
        this->collisionData.lastWitness.erase(otherID);
    }
}
void DynamicConvexPolygon::removeAllWitnessInformation() { this->collisionData.lastWitness.clear(); }

void DynamicConvexPolygon::applyForce(const Vector &force, const Vector &point) {
    const Vector newLinearMomentum = this->dynamicProperties.linearMomentum + force;
    const floatType newAngularMomentum = this->dynamicProperties.angularMomentum + point.cross(force);
    this->setLinearMomentum(newLinearMomentum);
    this->setAngularMomentum(newAngularMomentum);
}

void DynamicConvexPolygon::updateAfterMovementOrRotation() {
    this->recalculateWorldSpaceVertices();
    this->recalculateCurrentBoundingBox();
    this->recalculateSweptBoundingBox();
    // TODO this->recalculateProjections(); (?)
}
