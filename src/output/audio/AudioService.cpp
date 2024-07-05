//
// Created by Aleksis Vezenkov on 30.05.24.
//

#include "AudioService.hpp"

#include <mutex>

void AudioService::playSound(const std::string &soundName) const {
    try {
        PlaySound(resourceManager.getSound(soundName));
        spdlog::debug("Playing a sound with name: {}", soundName);
    } catch (...) {
        spdlog::error("Sound with the name {} was used, but it does not exist.", soundName);
    }
}

AudioService::AudioService(ResourceManager &resourceManager) : resourceManager(resourceManager){};

AudioService::~AudioService() { resourceManager.unloadAllSounds(); }