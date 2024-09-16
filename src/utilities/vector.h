#pragma once

#include "../game/GameProperties.hpp"
#include "raylib.h"
#include <optional>
/**
 * This struct represents a vector in two dimensional space with a x and a y coordinate.
 */
struct Vector {
    floatType x;
    floatType y;

    /**
     * Construct a vector from a given Raylib vector
     *
     * @param raylibVector
     */
    static Vector fromRaylibVector2(const Vector2 &raylibVector);

    /**
     * construct a Raylib vector from this vector.
     */
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
     * Computes the distance between the x-coordinates of this vector and the other vector.
     *
     * @param other
     * @return x-distance
     */
    floatType xDistance(Vector const &other) const;

    /**
     * Computes the distance between the y-coordinates of this vector and the other vector.
     *
     * @param other
     * @return y-distance
     */
    floatType yDistance(Vector const &other) const;

    /**
     * This method calculates the length of a vector as the distance to (0,0).
     * @return
     */
    floatType length() const;

    Vector reflectOnNormal(Vector const &normal, floatType rockTerrainDamping) const;
    /**
     * Calculates a vector with the same direction as the given one and length one.
     *
     * @return normalized vector
     */
    Vector normalize() const;

    /**
     * Performs simple linear interpolation
     *
     * @param xPos
     * @param left
     * @param right
     * @return
     */
    static floatType linearInterpolation(floatType xPos, Vector left, Vector right);

    bool operator<(const Vector &other) const;
    bool operator!=(const Vector &other) const;
    bool operator==(const Vector &other) const;

    /**
     * Dot Product. Commutative.
     * @param other
     * @return
     */
    floatType dot(const Vector &other) const;

    /**
     * Cross Product. Non-commutative.
     *
     * @param other
     * @return
     */
    floatType cross(const Vector &other) const;

    /**
     * Checks whether this vector is equal to another vector within a given relative error of eps.
     * The error is always relative to the vector with the bigger length.
     *
     * @param other
     * @param eps
     * @return
     */
    bool approxEq(const Vector &other, floatType eps) const;

    /**
     * Returns the projection of this vector onto the other vector.
     *
     * @param other
     * @return
     */
    Vector projectOn(const Vector &other) const;

    /**
     * Rotates this vector by the angle given in radians. Uses the rotation matrix:
     *      (cos(angle), -sin(angle)
     *      sin(angle), cos(angle))
     *
     * @param angle
     * @return
     */
    Vector rotateByAngle(floatType angle) const;

    /**
     * Rotates this vector by a random angle in the range [minAngle, maxAngle].
     *
     * @param minAngle
     * @param maxAngle
     * @return
     */
    Vector rotateByRandomAngle(floatType minAngle, floatType maxAngle) const;

    /**
     * Rotates this vector by an angle in the range Pi*[-randomness, randomness]
     *
     * @param randomness
     * @return rotated vector
     */
    Vector randomRotation(floatType randomness) const;

    /**
     * Returns the angle between {0, 1} and this vector. Positive, if this vector is in counterclockwise direction of
     * {0, 1}, negative else.
     * TODO the denominator is a bit unclear
     *
     * @return
     */
    floatType getRelativeAngle() const;

    /**
     * Returns the angle between the first and the second vector. Positive, if the second vector is in counterclockwise
     * direction of the first vector, negative else.
     *
     * @param first
     * @param second
     * @return
     */
    static floatType getRelativeAngle(const Vector &first, const Vector &second);

    /**
     * Returns a random angle that is in the range Pi*[-randomness, randomness].
     *
     * @param randomness
     * @return
     */
    static floatType getRandomAngle(floatType randomness);

    /**
     * Maps the given angle to the range [0, 2*PI] by adding or subtracting multiples of PI as needed.
     *
     * @return
     */
    static floatType mapAngleToRange(floatType angle);

    /**
     * Returns a vector with length 1, pointing in the direction specified by the angle counterclockwise from (1,0)
     *
     * @param angle
     * @return
     */
    static Vector convertAngleToVector(floatType angle);

    /**
     * Computes the slope of this vector.
     * Precondition: the x component of this vecotr must not be zero
     *
     * @return
     */
    floatType computeSlope() const;
};