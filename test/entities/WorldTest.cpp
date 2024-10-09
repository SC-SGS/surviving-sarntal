//
// Created by Anietta Weckauff on 07.07.24.
//
#include "../../src/entities/World.h"
#include "MockAudioService.h"
#include "MockResourceManager.h"

#include <gtest/gtest.h>

class WorldTestFixture : public testing::Test {
  protected:
    std::unique_ptr<World> world;
    ConfigManager &configManager = ConfigManager::getInstance();
    GameConstants gameConstants = configManager.getGameConstants();
    ResourceManager resourceManager = ResourceManager(configManager);
    std::unique_ptr<Terrain> terrain;
    std::unique_ptr<MockResourceManager> mockResourceManager;
    std::unique_ptr<MockAudioService> mockAudioService;
    std::unique_ptr<Hiker> hiker;
    std::unique_ptr<Monster> monster;
    std::unique_ptr<Inventory> inventory;

    void SetUp() override {
        mockResourceManager = std::make_unique<MockResourceManager>(ConfigManager::getInstance());
        mockAudioService = std::make_unique<MockAudioService>(*mockResourceManager);
        ON_CALL(*mockAudioService, playSound("use-kaiserschmarrn")).WillByDefault(testing::Return());
        ON_CALL(*mockAudioService, playSound("use-coin")).WillByDefault(testing::Return());
        ON_CALL(*mockAudioService, playSound("use-duck")).WillByDefault(testing::Return());
        terrain = std::make_unique<Terrain>(Terrain::getEmptyTerrain(
            gameConstants.hikerConstants, gameConstants.terrainConstants, resourceManager, {{0.0, 0.0}, {10.0, 10.0}}));
        hiker = std::make_unique<Hiker>(Vector{0, 0}, *mockAudioService, gameConstants.hikerConstants);
        monster = std::make_unique<Monster>(gameConstants);
        inventory = std::make_unique<Inventory>(*mockAudioService, gameConstants.itemsConstants);
        world = std::make_unique<World>(*terrain, *hiker, *inventory, *monster, *mockAudioService, gameConstants);
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

TEST_F(WorldTestFixture, IsOutOfScopeTest1) {
    Hiker &hiker = world->getHiker();
    const Vector position = {-gameConstants.terrainConstants.bufferLeft - 5, world->getHiker().getPosition().y};
    hiker.setPosition(position);
    EXPECT_TRUE(world->isOutOfScope(hiker));
}

TEST_F(WorldTestFixture, IsOutOfScopeTest2) {
    const Vector position = {graphics::SCREEN_WIDTH_IN_PIXEL / graphics::UNIT_TO_PIXEL_RATIO,
                             graphics::SCREEN_HEIGHT_IN_PIXEL / graphics::UNIT_TO_PIXEL_RATIO};
    const auto schmarrn = ItemDto{"kaiserschmarrn", KAISERSCHMARRN, true, false, true, 2};
    const Item item = {KAISERSCHMARRN, position, gameConstants.itemsConstants.itemBaseHeight, schmarrn};
    const bool outOfScope = world->isOutOfScope(item);
    EXPECT_TRUE(outOfScope);
    const Item item2 = {KAISERSCHMARRN, {0, 0}, gameConstants.itemsConstants.itemBaseHeight, schmarrn};
    const bool outOfScope2 = world->isOutOfScope(item2);
    EXPECT_TRUE(outOfScope2);
    Vector position2 = {graphics::SCREEN_WIDTH_IN_PIXEL / graphics::UNIT_TO_PIXEL_RATIO,
                        graphics::SCREEN_HEIGHT_IN_PIXEL / graphics::UNIT_TO_PIXEL_RATIO};
    Item item3 = {KAISERSCHMARRN, position2, gameConstants.itemsConstants.itemBaseHeight,
                  configManager.getItems()[KAISERSCHMARRN]};
    EXPECT_TRUE(world->isOutOfScope(item3));
}

TEST_F(WorldTestFixture, AddRockTest) {
    EXPECT_TRUE(world->getRocks().empty());
    /**const Rock rock = {{0, 0},
                       std::vector<Vector>{{-2, -4 / 3.0}, {1, -4 / 3.0}, {1, 8 / 3.0}},
                       {{0, 0}, {1, 0}, {0, 1}, {0, 0}},
                       1,
                       0,
                       DynamicProperties()};
    world->addRock(rock);
    EXPECT_TRUE(world->getRocks().size() == 1);*/
}

TEST_F(WorldTestFixture, AddItemTest) {
    EXPECT_TRUE(world->getItems().empty());
    const Item item = {
        KAISERSCHMARRN, {0, 0}, gameConstants.itemsConstants.itemBaseHeight, configManager.getItems()[KAISERSCHMARRN]};
    world->addItem(item);
    EXPECT_TRUE(world->getItems().size() == 1);
}

TEST_F(WorldTestFixture, UseKaiserschmarrnTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("eat")).WillByDefault(testing::Return());
    EXPECT_CALL(mockAudioService, playSound("eat")).Times(1);
    Terrain terrain = Terrain::getEmptyTerrain(gameConstants.hikerConstants, gameConstants.terrainConstants,
                                               resourceManager, {{0.0, 0.0}, {10.0, 10.0}});
    Hiker hiker(Vector{0, 0}, mockAudioService, gameConstants.hikerConstants);
    Monster monster(gameConstants);
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
    ON_CALL(mockAudioService, playSound("use-coin")).WillByDefault(testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-coin")).Times(1);
    Terrain terrain = Terrain::getEmptyTerrain(gameConstants.hikerConstants, gameConstants.terrainConstants,
                                               resourceManager, {{0.0, 0.0}, {10.0, 10.0}});
    Hiker hiker(Vector{0, 0}, mockAudioService, gameConstants.hikerConstants);
    Monster monster(gameConstants);
    Inventory inventory(mockAudioService, gameConstants.itemsConstants);
    World world(terrain, hiker, inventory, monster, mockAudioService, gameConstants);
    EXPECT_EQ(world.getCoinScore(), 0);
    world.useItem(COIN);
    EXPECT_EQ(world.getCoinScore(), gameConstants.itemsConstants.coinScore);
}

TEST_F(WorldTestFixture, UseDuckTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("use-duck")).WillByDefault(testing::Return());
    EXPECT_CALL(mockAudioService, playSound("use-duck")).Times(1);
    Terrain terrain = Terrain::getEmptyTerrain(gameConstants.hikerConstants, gameConstants.terrainConstants,
                                               resourceManager, {{0.0, 0.0}, {10.0, 10.0}});
    Hiker hiker(Vector{0, 0}, mockAudioService, gameConstants.hikerConstants);
    Monster monster(gameConstants);
    Inventory inventory(mockAudioService, gameConstants.itemsConstants);
    World world(terrain, hiker, inventory, monster, mockAudioService, gameConstants);
    world.useItem(DUCK_ITEM);
}
