#pragma once

#include "GameConstants.hpp"
#include "raylib.h"

/**
 * This struct represents a vector in two dimensional space with a x and a y coordinate.
 */
struct Vector {
    floatType x, y;

    explicit operator Vector2() const;

    /**
     * This method sets the x value.
     * @param xValue
     */
    void setX(floatType xValue);

    /**
     * This method sets the y value.
     * @param yValue
     */
    void setY(floatType yValue);

    /**
     * This method returns a new vector which is the addition of the current and the given vector.
     * @param other
     * @return addition of vectors
     */
    Vector operator+(Vector const &other) const;

    /**
     * This method adds a float number to a vector by adding the number to the x and
     * y coordinate separately.
     * @param scalar
     * @return vector {x + scalar, y + scalar}
     */
    Vector operator+(floatType scalar) const;

    /**
     * This method adds a vector by adding the x and y values from the other vector to the current x and y values.
     * @param other
     */
    void operator+=(Vector const &other);

    /**
     * This method returns a new vector which is the subtraction of the current and the given other vector.
     * @param other
     * @return difference of vectors
     */
    Vector operator-(Vector const &other) const;

    /**
     * This method subtracts a vector by subtracting the x and y value of the other vector from the current x and y
     * values.
     * @param other
     */
    void operator-=(Vector const &other);

    /**
     * This method multiplies the current x and y value with a given float number.
     * @param scalar
     * @return vector multiplied with scalar
     */
    Vector operator*(floatType scalar) const;

    /**
     * This method multiplies the current x value with the x value of the given vector
     * and multiplies the current y value with the y value of the given vector and adds the two results.
     * @param other
     * @return product of the vectors
     */
    floatType operator*(Vector const &other) const;

    /**
     * This method returns a vector where the current x and y values are divided by the given float scalar.
     * @param scalar
     * @return vector divided by scalar
     */
    Vector operator/(floatType scalar) const;

    /**
     * This method divides the current x and y values by a given float scalar.
     * @param scalar
     */
    void operator/=(floatType scalar);

    /**
     * This method calculates the distance between the current and a given vector useing
     * the Pythagorean theorem.
     * @param other
     * @return
     */
    floatType distanceTo(Vector const &other) const;

    /**
     * This method calculates the length of a vector as the distance to (0,0).
     * @return
     */
    floatType length() const;

    Vector reflectOnNormal(Vector const &normal) const;
};