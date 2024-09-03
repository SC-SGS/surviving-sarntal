#include "vector.h"
#include "RandomGenerator.hpp"
#include "cmath"
#include "raylib.h"
#include <optional>

Vector::operator Vector2() const { return Vector2{x, y}; }

Vector Vector::operator+(Vector const &other) const { return {x + other.x, y + other.y}; }

void Vector::operator+=(const Vector &other) {
    x += other.x;
    y += other.y;
}

Vector Vector::operator+(floatType scalar) const { return {x + scalar, y + scalar}; }

Vector Vector::operator-(Vector const &other) const { return {x - other.x, y - other.y}; }

void Vector::operator-=(const Vector &other) {
    x -= other.x;
    y -= other.y;
}

Vector Vector::operator*(floatType scalar) const { return {x * scalar, y * scalar}; }

floatType Vector::operator*(Vector const &other) const { return x * other.x + y * other.y; }

Vector Vector::operator/(floatType scalar) const { return {x / scalar, y / scalar}; }

void Vector::operator/=(floatType scalar) {
    x /= scalar;
    y /= scalar;
}
floatType Vector::distanceTo(const Vector &other) const {
    Vector deltaPos = *this - other;
    return std::sqrt(deltaPos * deltaPos);
}

floatType Vector::length() const { return this->distanceTo({0., 0.}); }

Vector Vector::reflectOnNormal(const Vector &normal, const floatType rockTerrainDamping) const {
    floatType normalVelocity = std::abs(*this * normal);
    return *this + normal * normalVelocity * (1.0f + rockTerrainDamping);
}
void Vector::setX(floatType xValue) { this->x = xValue; }
void Vector::setY(floatType yValue) { this->y = yValue; }

floatType Vector::linearInterpolation(const floatType xPos, const Vector left, const Vector right) {
    return ((xPos - left.x) * right.y + (right.x - xPos) * left.y) / (right.x - left.x);
}

Vector Vector::normalize() const {
    Vector current = *this;
    return current * (1.0f / this->length());
}
bool Vector::operator<(const Vector &other) const { return x < other.x || (x == other.x && y < other.y); }

bool Vector::operator==(const Vector &other) const { return this->x == other.x && this->y == other.y; }

bool Vector::operator!=(const Vector &other) const { return this->x != other.x || this->y != other.y; }

Vector Vector::rotateByAngle(floatType angle) const {
    return {static_cast<floatType>(cos(angle) * this->x - sin(angle) * this->y),
            static_cast<floatType>(sin(angle) * this->x + cos(angle) * this->y)};
}

Vector Vector::randomRotation(floatType randomness) const {
    floatType angle = Vector::getRandomAngle(randomness);
    return this->rotateByAngle(angle);
}

bool Vector::intersects(Vector firstStartpoint, Vector firstEndpoint, Vector secondStartpoint, Vector secondEndpoint) {
    Vector firstDirection = firstEndpoint - firstStartpoint;
    Vector secondDirection = secondEndpoint - secondStartpoint;
    floatType firstSlope = firstDirection.y / firstDirection.x;
    floatType firstYOffset = firstStartpoint.y + (-firstStartpoint.x / firstDirection.x) * firstDirection.y;

    floatType secondSlope = secondDirection.y / secondDirection.x;
    floatType secondYOffset = secondStartpoint.y + (-secondStartpoint.x / secondDirection.x) * secondDirection.y;

    if (firstSlope == secondSlope && firstStartpoint != secondStartpoint) {
        return false;
    }

    floatType intersectionX = (secondYOffset - firstYOffset) / (firstSlope - secondSlope);

    floatType firstRelativeTIntersection = (intersectionX - firstStartpoint.x) / firstDirection.x;
    floatType secondRelativeTIntersection = (intersectionX - secondStartpoint.x) / secondDirection.x;

    return 0 <= firstRelativeTIntersection && firstRelativeTIntersection <= 1 && 0 <= secondRelativeTIntersection &&
           secondRelativeTIntersection <= 1;
}

Vector Vector::intersectionPoint(Vector firstStartpoint, Vector firstEndpoint, Vector secondStartpoint,
                                 Vector secondEndpoint) {
    assert(intersects(firstStartpoint, firstEndpoint, secondStartpoint, secondEndpoint));
    Vector firstDirection = firstEndpoint - firstStartpoint;
    Vector secondDirection = secondEndpoint - secondStartpoint;
    floatType firstSlope = firstDirection.y / firstDirection.x;
    floatType firstYOffset = firstStartpoint.y + (-firstStartpoint.x / firstDirection.x) * firstDirection.y;

    floatType secondSlope = secondDirection.y / secondDirection.x;
    floatType secondYOffset = secondStartpoint.y + (-secondStartpoint.x / secondDirection.x) * secondDirection.y;

    floatType intersectionX = (secondYOffset - firstYOffset) / (firstSlope - secondSlope);
    floatType relativeTIntersection = (intersectionX - firstStartpoint.x) / firstDirection.x;

    return firstStartpoint + firstDirection * relativeTIntersection;
}

std::optional<Vector> Vector::getIntersection(Vector firstStartpoint, Vector firstEndpoint, Vector secondStartpoint,
                                              Vector secondEndpoint) {
    // Direction vectors of the lines
    floatType a1Entry = firstEndpoint.y - firstStartpoint.y;
    floatType b1Entry = firstStartpoint.x - firstEndpoint.x;
    floatType c1Entry = a1Entry * firstStartpoint.x + b1Entry * firstStartpoint.y;
    floatType a2Entry = secondEndpoint.y - secondStartpoint.y;
    floatType b2Entry = secondStartpoint.x - secondEndpoint.x;
    floatType c2Entry = a2Entry * secondStartpoint.x + b2Entry * secondStartpoint.y;
    // Determinant of the system
    floatType det = a1Entry * b2Entry - a2Entry * b1Entry;
    if (det == 0) { // Lines are parallel
        return std::nullopt;
    } else {
        // Cramer's rule to solve for the intersection point
        floatType xPos = (b2Entry * c1Entry - b1Entry * c2Entry) / det;
        floatType yPos = (a1Entry * c2Entry - a2Entry * c1Entry) / det;
        // Check if the intersection point lies on both line segments
        if ((std::min(firstStartpoint.x, firstEndpoint.x) <= xPos &&
             xPos <= std::max(firstStartpoint.x, firstEndpoint.x)) &&
            (std::min(firstStartpoint.y, firstEndpoint.y) <= yPos &&
             yPos <= std::max(firstStartpoint.y, firstEndpoint.y)) &&
            (std::min(secondStartpoint.x, secondEndpoint.x) <= xPos &&
             xPos <= std::max(secondStartpoint.x, secondEndpoint.x)) &&
            (std::min(secondStartpoint.y, secondEndpoint.y) <= yPos &&
             yPos <= std::max(secondStartpoint.y, secondEndpoint.y))) {
            return Vector{xPos, yPos};
        } else {
            return std::nullopt;
        }
    }
}

floatType Vector::getRelativeAngle() const {
    Vector normalized = this->normalize();
    floatType angle = std::acos(normalized.x);
    if (normalized.y > 0) {
        return angle;
    } else {
        return -angle;
    }
}

floatType Vector::getRelativeAngle(Vector first, Vector second) {
    floatType relativeAngleFirst = first.getRelativeAngle();
    floatType relativeAngleSecond = second.getRelativeAngle();
    return relativeAngleSecond - relativeAngleFirst;
}

floatType Vector::getRandomAngle(floatType randomness) {
    return PI * RandomGenerator::getInstance().getRandomRealNumber(-randomness, randomness);
}

Vector Vector::rotateByRandomAngle(floatType minAngle, floatType maxAngle) const {
    floatType randomAngle = RandomGenerator::getInstance().getRandomRealNumber(minAngle, maxAngle);
    return this->rotateByAngle(randomAngle);
}

floatType Vector::mapAngleToRange(floatType angle) {
    floatType correctedAngle = angle;
    while (correctedAngle > 2 * PI) {
        correctedAngle -= 2 * PI;
    }
    while (correctedAngle < 0) {
        correctedAngle += 2 * PI;
    }
    return correctedAngle;
}

Vector2 Vector::convertToRaylibRepresentation() { return {this->x, this->y}; }

floatType Vector::xDistance(const Vector &other) const { return fabsf(other.x - this->x); }

floatType Vector::yDistance(const Vector &other) const { return fabsf(other.y - this->y); }

Vector Vector::convertAngleToVector(floatType angle) {
    Vector basis = {1, 0};
    return basis.rotateByAngle(angle);
}

floatType Vector::computeSlope() const {
    assert(this->x != 0);
    return fabsf(this->y / this->x);
}
