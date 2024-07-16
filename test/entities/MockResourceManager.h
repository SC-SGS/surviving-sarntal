//
// Created by Anietta Weckauff on 07.07.24.
//

#ifndef SURVIVING_SARNTAL_MOCKRESOURCEMANAGER_H
#define SURVIVING_SARNTAL_MOCKRESOURCEMANAGER_H

#include "../../src/output/ResourceManager.h"
#include <gmock/gmock.h>

class MockResourceManager : public ResourceManager {
  public:
    MockResourceManager(ConfigManager &configManager) : ResourceManager(configManager) {} // NOLINT
    MOCK_METHOD(void, loadMusic, (), (override));
    MOCK_METHOD(void, loadTextures, (), (override));
};

#endif // SURVIVING_SARNTAL_MOCKRESOURCEMANAGER_H
