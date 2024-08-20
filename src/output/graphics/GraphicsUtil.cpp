//
// Created by six on 8/6/24.
//

#include "GraphicsUtil.h"

Vector GraphicsUtil::transformPosition(const Vector2 &vector, const Vector2 &offset) {
    const auto transformedVector = transformPosition(vector);
    return Vector{transformedVector.x - offset.x, transformedVector.y - offset.y};
}
Vector GraphicsUtil::transformPosition(const Vector2 &vector) {
    return Vector{vector.x, transformYCoordinate(vector.y)};
}

floatType GraphicsUtil::transformYCoordinate(const floatType yCoordinate) {
    return -yCoordinate + static_cast<floatType>(GetScreenHeight());
}

int GraphicsUtil::floorToNearest(const int number, const int placeValue) { return (number / placeValue) * placeValue; }