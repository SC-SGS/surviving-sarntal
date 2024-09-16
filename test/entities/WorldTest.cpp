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
    ConfigManager &configManager = ConfigManager::getInstance();
    GameConstants gameConstants = configManager.getGameConstants();

    void SetUp() override {
        MockResourceManager mockResourceManager(ConfigManager::getInstance());
        MockAudioService mockAudioService(mockResourceManager);
        ON_CALL(mockAudioService, playSound("use-kaiserschmarrn")).WillByDefault(::testing::Return());
        ON_CALL(mockAudioService, playSound("use-coin")).WillByDefault(::testing::Return());
        ON_CALL(mockAudioService, playSound("use-duck")).WillByDefault(::testing::Return());
        Terrain terrain = Terrain::getEmptyTerrain(gameConstants.hikerConstants, gameConstants.terrainConstants,
                                                   {{0.0, 0.0}, {10.0, 10.0}});
        Hiker hiker(Vector{0, 0}, mockAudioService, gameConstants.hikerConstants);
        Monster monster(gameConstants.hikerConstants);
        Inventory inventory(mockAudioService, gameConstants.itemsConstants);
        world = std::make_unique<World>(terrain, hiker, inventory, monster, mockAudioService, gameConstants);
    }

    void TearDown() override {}
};

// TEST_F(WorldTestFixture, GetNearbyItemsTest) {
//     Vector position = {world->getHiker().getPosition().x, world->getHiker().getPosition().y +
//     this->world->getHiker().getHeight() / 2}; const Item item = {KAISERSCHMARRN, position}; world->addItem(item);
//     std::list<std::shared_ptr<Item>> nearbyItems = world->getNearbyItems();
//     EXPECT_EQ(nearbyItems.size(), 1);
//     world->getItems().clear();
// }

TEST_F(WorldTestFixture, IsOutOfScopeTest) {
    Hiker &hiker = world->getHiker();
    Vector position = {-gameConstants.terrainConstants.bufferLeft - 5, world->getHiker().getPosition().y};
    hiker.setPosition(position);
    EXPECT_TRUE(world->isOutOfScope(hiker));
    Vector position2 = {graphics::SCREEN_WIDTH / graphics::UNIT_TO_PIXEL_RATIO,
                        graphics::SCREEN_HEIGHT / graphics::UNIT_TO_PIXEL_RATIO};
    Item item = {KAISERSCHMARRN, position2, gameConstants.itemsConstants.itemBaseHeight,
                 configManager.getItems()[KAISERSCHMARRN]};
    EXPECT_FALSE(world->isOutOfScope(item));
}

TEST_F(WorldTestFixture, AddRockTest) {
    EXPECT_TRUE(world->getRocks().empty());
    const Rock rock = {{0, 0},
                       std::vector<Vector>{{-2, -4 / 3.0}, {1, -4 / 3.0}, {1, 8 / 3.0}},
                       {{0, 0}, {1, 0}, {0, 1}, {0, 0}},
                       1,
                       0,
                       DynamicProperties()};
    world->addRock(rock);
    EXPECT_TRUE(world->getRocks().size() == 1);
}

TEST_F(WorldTestFixture, AddItemTest) {
    EXPECT_TRUE(world->getItems().empty());
    Item item = {
        KAISERSCHMARRN, {0, 0}, gameConstants.itemsConstants.itemBaseHeight, configManager.getItems()[KAISERSCHMARRN]};
    world->addItem(item);
    EXPECT_TRUE(world->getItems().size() == 1);
}

TEST_F(WorldTestFixture, UseKaiserschmarrnTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("use-kaiserschmarrn")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-kaiserschmarrn")).Times(1);
    Terrain terrain = Terrain::getEmptyTerrain(gameConstants.hikerConstants, gameConstants.terrainConstants,
                                               {{0.0, 0.0}, {10.0, 10.0}});
    Hiker hiker(Vector{0, 0}, mockAudioService, gameConstants.hikerConstants);
    Monster monster(gameConstants.hikerConstants);
    Inventory inventory(mockAudioService, gameConstants.itemsConstants);
    World world(terrain, hiker, inventory, monster, mockAudioService, gameConstants);
    world.getHiker().setHealthPoints(0);
    EXPECT_EQ(world.getHiker().getHealthPoints(), 0);
    world.useItem(KAISERSCHMARRN);
    EXPECT_EQ(world.getHiker().getHealthPoints(), gameConstants.itemsConstants.kaiserschmarrnHealthRestoration);
}

TEST_F(WorldTestFixture, UseCoinTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("use-coin")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-coin")).Times(1);
    Terrain terrain = Terrain::getEmptyTerrain(gameConstants.hikerConstants, gameConstants.terrainConstants,
                                               {{0.0, 0.0}, {10.0, 10.0}});
    Hiker hiker(Vector{0, 0}, mockAudioService, gameConstants.hikerConstants);
    Monster monster(gameConstants.hikerConstants);
    Inventory inventory(mockAudioService, gameConstants.itemsConstants);
    World world(terrain, hiker, inventory, monster, mockAudioService, gameConstants);
    EXPECT_EQ(world.getCoinScore(), 0);
    world.useItem(COIN);
    EXPECT_EQ(world.getCoinScore(), gameConstants.itemsConstants.coinScore);
}

TEST_F(WorldTestFixture, UseDuckTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("use-duck")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-duck")).Times(1);
    Terrain terrain = Terrain::getEmptyTerrain(gameConstants.hikerConstants, gameConstants.terrainConstants,
                                               {{0.0, 0.0}, {10.0, 10.0}});
    Hiker hiker(Vector{0, 0}, mockAudioService, gameConstants.hikerConstants);
    Monster monster(gameConstants.hikerConstants);
    Inventory inventory(mockAudioService, gameConstants.itemsConstants);
    World world(terrain, hiker, inventory, monster, mockAudioService, gameConstants);
    world.useItem(DUCK_ITEM);
}
