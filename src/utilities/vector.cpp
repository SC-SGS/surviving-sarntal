#include "vector.h"
#include "cmath"
#include "raylib.h"

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

Vector Vector::reflectOnNormal(const Vector &normal) const {
    floatType normalVelocity = std::abs(*this * normal);
    return *this + normal * normalVelocity * (1. + ROCK_TERRAIN_DAMPING);
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
