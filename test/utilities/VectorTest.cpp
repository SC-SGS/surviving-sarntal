//
// Created by Anietta Weckauff on 17.06.24.
//

#include "../src/utilities/vector.h"
#include <gtest/gtest.h>

class VectorTestFixture : public ::testing::Test {
  protected:
    Vector vector1 = {1, 2};
    Vector vector2 = {3, 4};
    Vector vector3 = {8, 4};

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(VectorTestFixture, ScalarAdditionTest) {
    Vector result = vector1 + 5;
    EXPECT_EQ(result.x, 6);
    EXPECT_EQ(result.y, 7);
}

TEST_F(VectorTestFixture, VectorAdditionTest) {
    Vector result = vector1 + vector2;
    EXPECT_EQ(result.x, 4);
    EXPECT_EQ(result.y, 6);
}

TEST_F(VectorTestFixture, VectorAdditionTest2) {
    vector1 += {2, 3};
    EXPECT_EQ(vector1.x, 3);
    EXPECT_EQ(vector1.y, 5);
}

TEST_F(VectorTestFixture, VectorSubtractionTest) {
    Vector result = vector2 - vector1;
    EXPECT_EQ(result.x, 2);
    EXPECT_EQ(result.y, 2);
}

TEST_F(VectorTestFixture, VectorSubtractionTest2) {
    vector2 -= {1, 1};
    EXPECT_EQ(vector2.x, 2);
    EXPECT_EQ(vector2.y, 3);
}

TEST_F(VectorTestFixture, VectorMultiplicationScalarTest) {
    Vector result = vector1 * 3;
    EXPECT_EQ(result.x, 3);
    EXPECT_EQ(result.y, 6);
}

TEST_F(VectorTestFixture, VectorMultiplicationTest) {
    floatType result = vector2 * vector1;
    EXPECT_EQ(result, 11);
}

TEST_F(VectorTestFixture, VectorDivisionScalarTest) {
    Vector result = vector3 / 2;
    EXPECT_EQ(result.x, 4);
    EXPECT_EQ(result.y, 2);
}

TEST_F(VectorTestFixture, VectorDivisionTest) {
    vector3 /= 2;
    EXPECT_EQ(vector3.x, 4);
    EXPECT_EQ(vector3.y, 2);
}

TEST_F(VectorTestFixture, LengthTest) { EXPECT_EQ(vector2.length(), 5); }
