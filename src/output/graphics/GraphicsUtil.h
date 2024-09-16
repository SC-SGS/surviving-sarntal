//
// Created by six on 8/6/24.
//

#ifndef SURVIVING_SARNTAL_GRAPHICSUTIL_H
#define SURVIVING_SARNTAL_GRAPHICSUTIL_H

#include "../../utilities/vector.h"
#include "raylib.h"

class GraphicsUtil {
  public:
    /**
     * Transforms a vector to a position on the screen.
     * @param vector is the vector to be transformed
     * @param offset is the offset to be added to the vector
     * @return a vector representing the position on the screen
     */
    static Vector transformPosition(const Vector2 &vector, const Vector2 &offset);

    /**
     * Transforms a vector to a position on the screen.
     * TODO this takes a Vector2 (Raylib) and returns a Vector...
     * @param vector is the vector to be transformed
     * @return a vector representing the position on the screen
     */
    static Vector transformPosition(const Vector2 &vector);

    /**
     * Transforms a y-Coordinate to a correct y-position on the screen.
     * @param floatType is the y-Coordinate to be transformed
     * @return a floatType representing the y-position on the screen
     */
    static floatType transformYCoordinate(floatType yCoordinate);

    /**
     * Floors a number to the nearest place value.
     * @param number is the number to be floored
     * @param placeValue is the place value to which the number should be floored
     * @return an integer representing the floored number.
     */
    static int floorToNearest(int number, int placeValue);
};

#endif // SURVIVING_SARNTAL_GRAPHICSUTIL_H
