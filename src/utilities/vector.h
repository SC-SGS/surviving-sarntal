#pragma once

#include "GameConstants.hpp"
#include "raylib.h"

struct Position;

struct Velocity;

struct Vector {
    floatType x, y;

    explicit operator Vector2() const;

    explicit operator Position() const;

    explicit operator Velocity() const;

    void setX(floatType xValue);

    void setY(floatType yValue);

    Velocity operator+(Vector const &) const;

    Vector operator+(floatType) const;

    void operator+=(Vector const &);

    Vector operator-(Vector const &) const;

    void operator-=(Vector const &);

    Vector operator*(floatType) const;

    floatType operator*(Vector const &) const;

    Vector operator/(floatType) const;

    void operator/=(floatType);

    [[nodiscard]] floatType distanceTo(Vector const &) const;

    [[nodiscard]] floatType length() const;

    [[nodiscard]] Vector reflectOnNormal(Vector const &) const;
};

struct Position : Vector {};

struct Velocity : Vector {};

struct Rotation {
    floatType angular_velocity;
    floatType angular_offset;
};