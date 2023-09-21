#pragma once

struct Vector {
    float x, y;

    Vector operator+(Vector const &other) const {
        return {.x = x + other.x, .y = y + other.y};
    }

    Vector operator-(Vector const &other) const {
        return {.x = x - other.x, .y = y - other.y};
    }

    Vector operator*(float scalar) const {
        return {.x = x * scalar, .y = y * scalar};
    }

    float operator*(Vector const &other) const {
        return x * other.x + y * other.y;
    }
};

struct Position : Vector {};

struct Velocity : Vector {};

struct Radius {
    float value;
};

struct Rock {};

struct AppInfo {
    bool isRunning = true;
};