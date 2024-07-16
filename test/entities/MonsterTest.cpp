//
// Created by Anietta Weckauff on 07.07.24.
//
#include "../../src/entities/Monster.h"
#include <gtest/gtest.h>

class MonsterTestFixture : public ::testing::Test {
  protected:
    Monster monster;

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(MonsterTestFixture, SetXPositionTest) {
    monster.setXPosition(30);
    EXPECT_EQ(monster.getPosition().x, 30);
}

TEST_F(MonsterTestFixture, GetXPositionTest) {
    Vector position = {100, 20};
    monster.setPosition(position);
    EXPECT_EQ(monster.getXPosition(), 100);
}
