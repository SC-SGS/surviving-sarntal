//
// Created by six on 8/6/24.
//

#include "GraphicsUtil.h"

Vector GraphicsUtil::transformPosition(const Vector2 &vector, const Vector2 &offset) {
    Vector2 offsetPosition{vector.x - offset.x, vector.y + offset.y};
    return transformPosition(offsetPosition);
}
Vector GraphicsUtil::transformPosition(const Vector2 &vector) {
    return Vector{vector.x * graphics::UNIT_TO_PIXEL_RATIO, transformYCoordinate(vector.y)};
}

floatType GraphicsUtil::transformYCoordinate(const floatType yCoordinate) {
    return -(yCoordinate * graphics::UNIT_TO_PIXEL_RATIO) + static_cast<floatType>(GetScreenHeight());
}

int GraphicsUtil::floorToNearest(const int number, const int placeValue) { return (number / placeValue) * placeValue; }