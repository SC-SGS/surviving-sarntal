#pragma once

#include "../utils/game_constants.h"
#include "raylib.h"

struct Position;

struct Velocity;

struct Vector {
    float_type x, y;

    explicit operator Vector2() const;

    explicit operator Position() const;

    explicit operator Velocity() const;

    Velocity operator+(Vector const &) const;

    Vector operator+(float_type) const;

    void operator+=(Vector const &);

    Vector operator-(Vector const &) const;

    void operator-=(Vector const &);

    Vector operator*(float_type) const;

    float_type operator*(Vector const &) const;

    Vector operator/(float_type) const;

    void operator/=(float_type);

    [[nodiscard]] float_type distanceTo(Vector const &) const;

    [[nodiscard]] float_type length() const;

    [[nodiscard]] Vector reflectOnNormal(Vector const &) const;
};

struct Position : Vector {};

struct Velocity : Vector {};

struct Radius {
    float_type value;
};

struct Rotation {
    float_type angular_velocity;
    float_type angular_offset;
};

struct Rock {};

struct Exploding {};
