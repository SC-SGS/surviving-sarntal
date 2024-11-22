//
// Created by Anietta Weckauff on 07.07.24.
//

#ifndef SURVIVING_SARNTAL_MOCKAUDIOSERVICE_H
#define SURVIVING_SARNTAL_MOCKAUDIOSERVICE_H

#include "../../src/output/audio/AudioService.hpp"
#include <gmock/gmock.h>

class MockAudioService : public AudioService {
  public:
    explicit MockAudioService(ResourceManager &resourceManager) : AudioService(resourceManager) {}
    MOCK_METHOD(void, playSound, (const std::string &soundName), (const, override));
    MOCK_METHOD(void, interruptSound, (const std::string &soundName), (const, override));
};

#endif // SURVIVING_SARNTAL_MOCKAUDIOSERVICE_H
