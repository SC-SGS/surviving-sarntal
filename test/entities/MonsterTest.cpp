//
// Created by Anietta Weckauff on 07.07.24.
//
#include "../../src/entities/Monster.h"
#include "../../src/utilities/ConfigManager.hpp"
#include <gtest/gtest.h>

class MonsterTestFixture : public testing::Test {
  protected:
    ConfigManager &configManager = ConfigManager::getInstance();
    GameConstants gameConstants = configManager.getGameConstants();
    Monster monster{gameConstants};

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(MonsterTestFixture, SetXPositionTest) {
    monster.setXPosition(30);
    EXPECT_EQ(monster.getPosition().x, 30);
}

TEST_F(MonsterTestFixture, GetXPositionTest) {
    const Vector position = {100, 20};
    monster.setPosition(position);
    EXPECT_EQ(monster.getXPosition(), 100);
}
