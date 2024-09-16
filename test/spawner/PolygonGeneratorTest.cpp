//
// Created by bjoern on 9/4/24.
//

#include "PolygonGeneratorTest.h"

PolygonGeneratorTest::PolygonGeneratorTest()
    : rightTriangle(std::vector<Vector>{{0, 0}, {3, 0}, {3, 4}}),
      rightTriangleBodySpace(std::vector<Vector>{{-2, -4 / 3.0}, {1, -4 / 3.0}, {1, 8 / 3.0}}),
      square(std::vector<Vector>{{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}}),
      rectangle(std::vector<Vector>{{-3, -4}, {3, -4}, {3, 4}, {-3, 4}}) {}

TEST_F(PolygonGeneratorTest, AreaBaseCaseTriangle) {
    // When the triangle has the coordinates given above.
    const floatType area = calculateAreaBodySpace(rightTriangleBodySpace);
    // Then the area of said triangle should be 6
    ASSERT_EQ(6, area);
}

TEST_F(PolygonGeneratorTest, AreaBaseCaseSquare) {
    // When the triangle has the coordinates given above.
    const floatType area = calculateAreaBodySpace(square);
    // Then the area of said triangle should be 6
    ASSERT_EQ(1, area);
}

TEST_F(PolygonGeneratorTest, TriangleInertiaEdgeCaseRightTriangleOHB) {
    // When the triangle has the coordinates given above
    const floatType momentOfInertia =
        calculateInertiaOfTriangleRotatingAroundOriginP0(rightTriangle[1], rightTriangle[2], 1.0);
    // Then the inertia of that triangle should be 3*4^3 / 4 + 3^3 * 4 / 12 = 48 + 9 = 57
    ASSERT_EQ(57, momentOfInertia);
}

TEST_F(PolygonGeneratorTest, TriangleInertiaEdgeCaseRightTriangleOAH) {
    // When the triangle has the coordinates given above
    const floatType momentOfInertia =
        calculateInertiaOfTriangleRotatingAroundOriginP0(Vector{3, -4}, rightTriangle[1], 2.0);
    // Then the inertia of that triangle should be 2* (3*4^3 / 4 + 3^3 * 4 / 12) = 2*(48 + 9) = 114
    ASSERT_EQ(114, momentOfInertia);
}

TEST_F(PolygonGeneratorTest, TriangleInertiaVerticesNotAnticlockwiseError) {
    // When the vertices are given in a non-anticlockwise order
    // Then an exception should be thrown
    EXPECT_THROW(calculateInertiaOfTriangleRotatingAroundOriginP0(rightTriangle[2], rightTriangle[1], 1.0),
                 std::invalid_argument);
}

TEST_F(PolygonGeneratorTest, InertiaBaseCaseRectangle) {
    // When the rectangle above is given
    const floatType momentOfInertia = calculateInertiaRotCentroidBodySpace(rectangle, 1.0);
    // Then the result should be the sum of 8 of the right triangles given above, 4 of them with height 4 and width 3
    // and 4 of them with height 3 and width 4: 4*43 + 4*57= 172 + 228 = 400
    EXPECT_EQ(400, momentOfInertia);
}

TEST_F(PolygonGeneratorTest, InertiaVerticesNotAnticlockwiseError) {
    // When the vertices are given in a non-anticlockwise order
    std::vector<Vector> reversedVertices(4);
    std::reverse_copy(rectangle.begin(), rectangle.end(), reversedVertices.begin());
    // Then an exception should be thrown
    EXPECT_THROW(calculateInertiaRotCentroidBodySpace(reversedVertices, 1.0f), std::invalid_argument);
}
