//
// Created by Anietta Weckauff on 07.07.24.
//
#include "../../src/entities/World.h"
#include "MockAudioService.h"
#include "MockResourceManager.h"

#include <gtest/gtest.h>

class WorldTestFixture : public ::testing::Test {
  protected:
    std::unique_ptr<World> world;

    void SetUp() override {
        MockResourceManager mockResourceManager(ConfigManager::getInstance());
        MockAudioService mockAudioService(mockResourceManager);
        ON_CALL(mockAudioService, playSound("use-kaiserschmarrn")).WillByDefault(::testing::Return());
        ON_CALL(mockAudioService, playSound("use-coin")).WillByDefault(::testing::Return());
        ON_CALL(mockAudioService, playSound("use-duck")).WillByDefault(::testing::Return());
        Mountain mountain;
        Hiker hiker(Vector{0, 0}, mockAudioService);
        Monster monster;
        Inventory inventory(mockAudioService);
        world = std::make_unique<World>(mountain, hiker, inventory, monster, mockAudioService);
    }

    void TearDown() override {}
};

TEST_F(WorldTestFixture, GetNearbyItemsTest) {
    Vector position = {world->getHiker().getPosition().x, world->getHiker().getPosition().y};
    const Item item = {KAISERSCHMARRN, position};
    world->addItem(item);
    std::list<std::shared_ptr<Item>> nearbyItems = world->getNearbyItems();
    EXPECT_TRUE(nearbyItems.size() == 1);
    world->getItems().clear();
}

TEST_F(WorldTestFixture, IsOutOfScopeTest) {
    Hiker &hiker = world->getHiker();
    Vector position = {-Mountain::CHUNK_WIDTH - 5, world->getHiker().getPosition().y};
    hiker.setPosition(position);
    EXPECT_TRUE(world->isOutOfScope(hiker));
    Vector position2 = {graphics::SCREEN_WIDTH, graphics::SCREEN_HEIGHT};
    Item item = {KAISERSCHMARRN, position2};
    EXPECT_FALSE(world->isOutOfScope(item));
}

TEST_F(WorldTestFixture, AddRockTest) {
    EXPECT_TRUE(world->getRocks().empty());
    Rock rock = {{0, 0}, {1, 2}, 0.1, 0.1, 3};
    world->addRock(rock);
    EXPECT_TRUE(world->getRocks().size() == 1);
}

TEST_F(WorldTestFixture, AddItemTest) {
    EXPECT_TRUE(world->getItems().empty());
    Item item = {KAISERSCHMARRN, {0, 0}};
    world->addItem(item);
    EXPECT_TRUE(world->getItems().size() == 1);
}

TEST_F(WorldTestFixture, UseKaiserschmarrnTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("use-kaiserschmarrn")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-kaiserschmarrn")).Times(1);
    Mountain mountain;
    Hiker hiker(Vector{0, 0}, mockAudioService);
    Monster monster;
    Inventory inventory(mockAudioService);
    World world(mountain, hiker, inventory, monster, mockAudioService);
    world.getHiker().setHealthPoints(0);
    EXPECT_EQ(world.getHiker().getHealthPoints(), 0);
    world.useItem(KAISERSCHMARRN);
    EXPECT_EQ(world.getHiker().getHealthPoints(), KAISERSCHMARRN_HEALTH_RESTORATION);
}

TEST_F(WorldTestFixture, UseCoinTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("use-coin")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-coin")).Times(1);
    Mountain mountain;
    Hiker hiker(Vector{0, 0}, mockAudioService);
    Monster monster;
    Inventory inventory(mockAudioService);
    World world(mountain, hiker, inventory, monster, mockAudioService);
    EXPECT_EQ(world.getCoinScore(), 0);
    world.useItem(COIN);
    EXPECT_EQ(world.getCoinScore(), COIN_SCORE);
}

TEST_F(WorldTestFixture, UseDuckTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("use-duck")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-duck")).Times(1);
    Mountain mountain;
    Hiker hiker(Vector{0, 0}, mockAudioService);
    Monster monster;
    Inventory inventory(mockAudioService);
    World world(mountain, hiker, inventory, monster, mockAudioService);
    world.useItem(DUCK_ITEM);
}
