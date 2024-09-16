//
// Created by bjoern on 9/4/24.
//

#ifndef POLYGONGENERATORTEST_H
#define POLYGONGENERATORTEST_H

#include "../../src/spawner/PolygonGenerator.h"

#include <gtest/gtest.h>

class PolygonGeneratorTest : public testing::Test, public PolygonGenerator {
  protected:
    const std::vector<Vector> rightTriangle;
    const std::vector<Vector> rightTriangleBodySpace;
    const std::vector<Vector> square;
    const std::vector<Vector> rectangle;

    PolygonGeneratorTest();
};

#endif // POLYGONGENERATORTEST_H
