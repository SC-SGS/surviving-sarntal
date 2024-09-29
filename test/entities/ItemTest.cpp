//
// Created by Anietta Weckauff on 07.07.24.
//
#include "../src/entities/Item.hpp"
#include "../src/utilities/ConfigManager.hpp"
#include <gtest/gtest.h>

class ItemTestFixture : public ::testing::Test {
  protected:
    Vector position{};
    ConfigManager &configManager = ConfigManager::getInstance();
    GameConstants gameConstants = configManager.getGameConstants();

    void SetUp() override { position = {0, 0}; }

    void TearDown() override {}
};

TEST_F(ItemTestFixture, CreateKaiserschmarrnItemTest) {
    auto kaiserschmarrnItem =
        std::make_shared<Item>(KAISERSCHMARRN, position, gameConstants.itemsConstants.itemBaseHeight,
                               configManager.getItems()[KAISERSCHMARRN]);
    EXPECT_EQ(kaiserschmarrnItem->getItemType(), KAISERSCHMARRN);
    EXPECT_TRUE(kaiserschmarrnItem->canAutoCollect());
    EXPECT_FALSE(kaiserschmarrnItem->canUseOnPickUp());
    EXPECT_TRUE(kaiserschmarrnItem->shouldDropOnUse());
}

TEST_F(ItemTestFixture, CreateCoinItemTest) {
    auto coinItem = std::make_shared<Item>(COIN, position, gameConstants.itemsConstants.itemBaseHeight,
                                           configManager.getItems()[COIN]);
    EXPECT_EQ(coinItem->getItemType(), COIN);
    EXPECT_TRUE(coinItem->canAutoCollect());
    EXPECT_TRUE(coinItem->canUseOnPickUp());
    EXPECT_TRUE(coinItem->shouldDropOnUse());
}

TEST_F(ItemTestFixture, CreateDuckItemTest) {
    auto kaiserschmarrnItem = std::make_shared<Item>(DUCK_ITEM, position, gameConstants.itemsConstants.itemBaseHeight,
                                                     configManager.getItems()[DUCK_ITEM]);
    EXPECT_EQ(kaiserschmarrnItem->getItemType(), DUCK_ITEM);
    EXPECT_TRUE(kaiserschmarrnItem->canAutoCollect());
    EXPECT_FALSE(kaiserschmarrnItem->canUseOnPickUp());
    EXPECT_TRUE(kaiserschmarrnItem->shouldDropOnUse());
}
