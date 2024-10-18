#include "vector.h"
#include "RandomGenerator.hpp"
#include "cmath"
#include "raylib.h"
#include <optional>

Vector Vector::fromRaylibVector2(const Vector2 &raylibVector) { return {raylibVector.x, raylibVector.y}; };

Vector::operator Vector2() const { return Vector2{x, y}; }

Vector Vector::operator+(Vector const &other) const { return {x + other.x, y + other.y}; }

void Vector::operator+=(const Vector &other) {
    x += other.x;
    y += other.y;
}

Vector Vector::operator+(const floatType scalar) const { return {x + scalar, y + scalar}; }

Vector Vector::operator-(const Vector &other) const { return {x - other.x, y - other.y}; }

void Vector::operator-=(const Vector &other) {
    x -= other.x;
    y -= other.y;
}

Vector Vector::operator*(const floatType scalar) const { return {x * scalar, y * scalar}; }

floatType Vector::operator*(const Vector &other) const { return x * other.x + y * other.y; }

Vector Vector::operator/(const floatType scalar) const { return {x / scalar, y / scalar}; }

void Vector::operator/=(const floatType scalar) {
    x /= scalar;
    y /= scalar;
}
floatType Vector::distanceTo(const Vector &other) const {
    const Vector deltaPos = *this - other;
    return std::sqrt(deltaPos * deltaPos);
}

floatType Vector::length() const { return this->distanceTo({0., 0.}); }

floatType Vector::lengthSq() const { return this->dot(*this); }

Vector Vector::reflectOnNormalDampened(const Vector &normal, floatType terrainDampening) const {
    const Vector norm = normal.normalize();
    const float dotProduct = dot(norm);
    const Vector reflected = *this - norm * (2 * dotProduct);
    return reflected * (1.0f - terrainDampening);
}

Vector Vector::reflectOnNormal(const Vector &normal, const floatType rockTerrainDamping) const {
    const floatType normalVelocity = std::abs(*this * normal);
    return *this + normal * normalVelocity * (1.0f + rockTerrainDamping);
}
void Vector::setX(const floatType xValue) { this->x = xValue; }
void Vector::setY(const floatType yValue) { this->y = yValue; }

floatType Vector::linearInterpolation(const floatType xPos, const Vector left, const Vector right) {
    return ((xPos - left.x) * right.y + (right.x - xPos) * left.y) / (right.x - left.x);
}

Vector Vector::normalize() const {
    const Vector current = *this;
    return current * (1.0f / this->length());
}

Vector Vector::preCrossZScalar(floatType zValue) const { return {-zValue * this->y, zValue * this->x}; }

bool Vector::operator<(const Vector &other) const { return x < other.x || (x == other.x && y < other.y); }
bool Vector::operator!=(const Vector &other) const { return x != other.x || y != other.y; }
bool Vector::operator==(const Vector &other) const { return x == other.x && y == other.y; }

floatType Vector::dot(const Vector &other) const { return x * other.x + y * other.y; }
floatType Vector::cross(const Vector &other) const { return x * other.y - y * other.x; }
bool Vector::approxEq(const Vector &other, const floatType eps) const {
    if (*this == Vector{0, 0} || other == Vector{0, 0}) {
        return this->distanceTo(other) < eps;
    }
    return this->distanceTo(other) < eps * std::max(this->length(), other.length());
}

Vector Vector::projectOn(const Vector &other) const {
    const Vector normalizedOther = other.normalize();
    return normalizedOther * normalizedOther.dot(*this);
}

Vector Vector::rotateByAngle(const floatType angle) const {
    return {static_cast<floatType>(cos(angle) * this->x - sin(angle) * this->y),
            static_cast<floatType>(sin(angle) * this->x + cos(angle) * this->y)};
}

Vector Vector::randomRotation(const floatType randomness) const {
    const floatType angle = getRandomAngle(randomness);
    return this->rotateByAngle(angle);
}

floatType Vector::getRelativeAngle() const {
    const Vector normalized = this->normalize();
    const floatType angle = std::acos(normalized.x);
    if (normalized.y > 0) {
        return angle;
    } else {
        return -angle;
    }
}

floatType Vector::getRelativeAngle(const Vector &first, const Vector &second) {
    const floatType relativeAngleFirst = first.getRelativeAngle();
    const floatType relativeAngleSecond = second.getRelativeAngle();
    return relativeAngleSecond - relativeAngleFirst;
}

floatType Vector::getRandomAngle(const floatType randomness) {
    return PI * RandomGenerator::getInstance().getRandomRealNumber(-randomness, randomness);
}

Vector Vector::rotateByRandomAngle(const floatType minAngle, const floatType maxAngle) const {
    const floatType randomAngle = RandomGenerator::getInstance().getRandomRealNumber(minAngle, maxAngle);
    return this->rotateByAngle(randomAngle);
}

floatType Vector::mapAngleToRange(const floatType angle) {
    floatType correctedAngle = angle;
    while (correctedAngle > 2 * PI) {
        correctedAngle -= 2 * PI;
    }
    while (correctedAngle < 0) {
        correctedAngle += 2 * PI;
    }
    return correctedAngle;
}

floatType Vector::xDistance(const Vector &other) const { return fabsf(other.x - this->x); }

floatType Vector::yDistance(const Vector &other) const { return fabsf(other.y - this->y); }

Vector Vector::convertAngleToVector(const floatType angle) {
    const Vector basis = {1, 0};
    return basis.rotateByAngle(angle);
}

floatType Vector::computeSlope() const {
    assert(this->x != 0);
    return fabsf(this->y / this->x);
}

Vector Vector::getOutwardsNormalVectorForCCWEdge() const { return (Vector{this->y, -this->x} / this->length()); }
