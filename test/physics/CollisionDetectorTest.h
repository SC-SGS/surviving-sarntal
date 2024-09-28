//
// Created by bjoern on 9/22/24.
//

#ifndef COLLISIONDETECTORTEST_H
#define COLLISIONDETECTORTEST_H

#include "../../src/utilities/vector.h"
#include <gtest/gtest.h>

class CollisionDetectorTest : public testing::Test {
  protected:
    const std::vector<Vector> rightTriangle;
    const std::vector<Vector> rightTriangleBodySpace;
    const std::vector<Vector> square;
    const std::vector<Vector> rectangle;

    CollisionDetectorTest();
};

#endif // COLLISIONDETECTORTEST_H
