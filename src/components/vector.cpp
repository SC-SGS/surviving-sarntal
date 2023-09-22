#include "vector.h"
#include "cmath"
#include "raylib.h"

Vector::operator Vector2() const { return Vector2{x, y}; }

Vector::operator Position() const { return Position{x, y}; }

Vector::operator Velocity() const { return Velocity{x, y}; }

Velocity Vector::operator+(Vector const &other) const {
    return {x + other.x, y + other.y};
}

void Vector::operator+=(const Vector &other) {
    x += other.x;
    y += other.y;
}

Vector Vector::operator+(float_type other) const {
    return {x + other, y + other};
}

Vector Vector::operator-(Vector const &other) const {
    return {x - other.x, y - other.y};
}

void Vector::operator-=(const Vector &other) {
    x -= other.x;
    y -= other.y;
}

Vector Vector::operator*(float_type scalar) const {
    return {x * scalar, y * scalar};
}

float_type Vector::operator*(Vector const &other) const {
    return x * other.x + y * other.y;
}

Vector Vector::operator/(float_type scalar) const {
    return {x / scalar, y / scalar};
}

void Vector::operator/=(float_type scalar) {
    x /= scalar;
    y /= scalar;
}
float_type Vector::distanceTo(const Vector &other) const {
    Vector delta_pos = *this - other;
    return std::sqrt(delta_pos * delta_pos);
}

float_type Vector::length() const { return this->distanceTo({0., 0.}); }

Vector Vector::reflectOnNormal(const Vector &normal) const {
    float_type normal_velocity = std::abs(*this * normal);
    return *this + normal * normal_velocity * 2.;
}
