//
// Created by Anietta Weckauff on 07.07.24.
//

#include "../../src/entities/Hiker.h"
#include "MockAudioService.h"
#include "MockResourceManager.h"
#include "raylib.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class HikerTestFixture : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
    ConfigManager &configManager = ConfigManager::getInstance();
    ResourceManager resourceManager = ResourceManager(configManager);
    InputHandler inputHandler = InputHandler(resourceManager);
    GameConstants gameConstants = configManager.getGameConstants();
};

// test

TEST_F(HikerTestFixture, AddHealthPointsTest) {
    MockResourceManager mockResourceManager(configManager);
    MockAudioService mockAudioService(mockResourceManager);
    Hiker hiker(Vector{0, 0}, mockAudioService, inputHandler, gameConstants.hikerConstants);

    hiker.setHealthPoints(30);
    hiker.addHealthPoints(20);

    EXPECT_EQ(hiker.getHealthPoints(), 50);
}

TEST_F(HikerTestFixture, CrouchTest) {
    MockResourceManager mockResourceManager(configManager);
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("crouch")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("crouch")).Times(1);
    Hiker hiker(Vector{0, 0}, mockAudioService, inputHandler, gameConstants.hikerConstants);

    hiker.crouch();

    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::CROUCHED);
    EXPECT_EQ(hiker.getHeight(),
              gameConstants.hikerConstants.hikerCrouchRatio * gameConstants.hikerConstants.hikerSize);
    EXPECT_EQ(hiker.getWidth(),
              gameConstants.hikerConstants.hikerCrouchRatio * gameConstants.hikerConstants.hikerWidth);
}

TEST_F(HikerTestFixture, UncrouchTest) {
    MockResourceManager mockResourceManager(configManager);
    MockAudioService mockAudioService(mockResourceManager);
    Hiker hiker(Vector{0, 0}, mockAudioService, inputHandler, gameConstants.hikerConstants);

    hiker.uncrouch();

    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::MOVING);
    EXPECT_EQ(hiker.getHeight(), gameConstants.hikerConstants.hikerSize);
    EXPECT_EQ(hiker.getWidth(), gameConstants.hikerConstants.hikerWidth);
}

TEST_F(HikerTestFixture, JumpOnceTest) {
    MockResourceManager mockResourceManager(configManager);
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("jump")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("jump")).Times(1);
    Hiker hiker(Vector{0, 0}, mockAudioService, inputHandler, gameConstants.hikerConstants);

    hiker.jump();

    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::IN_AIR);
    EXPECT_EQ(hiker.getVelocity().y, gameConstants.hikerConstants.jumpVelocity);
    EXPECT_EQ(hiker.getHikerMovement().getCanJumpAgain(), true);
}

TEST_F(HikerTestFixture, JumpTwiceTest) {
    MockResourceManager mockResourceManager(configManager);
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("jump")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("jump")).Times(2);
    Hiker hiker(Vector{0, 0}, mockAudioService, inputHandler, gameConstants.hikerConstants);

    hiker.jump();

    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::IN_AIR);
    EXPECT_EQ(hiker.getVelocity().y, gameConstants.hikerConstants.jumpVelocity);

    hiker.jump();

    EXPECT_EQ(hiker.getHikerMovement().getCanJumpAgain(), false);
}
