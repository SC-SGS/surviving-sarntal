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
};

// test

TEST_F(HikerTestFixture, AddHealthPointsTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    Hiker hiker(Vector{0, 0}, mockAudioService);
    hiker.setHealthPoints(30);
    hiker.addHealthPoints(20);
    EXPECT_EQ(hiker.getHealthPoints(), 50);
}

TEST_F(HikerTestFixture, CrouchTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("crouch")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("crouch")).Times(1);
    Hiker hiker(Vector{0, 0}, mockAudioService);
    hiker.crouch();
    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::CROUCHED);
    EXPECT_EQ(hiker.getHeight(), DUCKED_HIKER_HEIGHT);
    EXPECT_EQ(hiker.getWidth(), DUCKED_HIKER_WIDTH);
}

TEST_F(HikerTestFixture, UncrouchTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    Hiker hiker(Vector{0, 0}, mockAudioService);
    hiker.uncrouch();
    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::MOVING);
    EXPECT_EQ(hiker.getHeight(), HIKER_HEIGHT);
    EXPECT_EQ(hiker.getWidth(), HIKER_WIDTH);
}

TEST_F(HikerTestFixture, JumpOnceTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("jump")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("jump")).Times(1);
    Hiker hiker(Vector{0, 0}, mockAudioService);
    hiker.jump();
    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::IN_AIR);
    EXPECT_EQ(hiker.getVelocity().y, JUMP_VELOCITY_CONSTANT);
    EXPECT_EQ(hiker.getHikerMovement().getCanJumpAgain(), true);
}

TEST_F(HikerTestFixture, JumpTwiceTest) {
    MockResourceManager mockResourceManager(ConfigManager::getInstance());
    MockAudioService mockAudioService(mockResourceManager);
    ON_CALL(mockAudioService, playSound("jump")).WillByDefault(::testing::Return());
    EXPECT_CALL(mockAudioService, playSound("jump")).Times(2);
    Hiker hiker(Vector{0, 0}, mockAudioService);
    hiker.jump();
    EXPECT_EQ(hiker.getHikerMovement().getState(), HikerMovement::IN_AIR);
    EXPECT_EQ(hiker.getVelocity().y, JUMP_VELOCITY_CONSTANT);
    hiker.jump();
    EXPECT_EQ(hiker.getHikerMovement().getCanJumpAgain(), false);
}
