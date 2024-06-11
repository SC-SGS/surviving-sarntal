//
// Created by Aleksis Vezenkov on 30.05.24.
//

#include "AudioService.hpp"

#include <mutex>

void AudioService::playSound(const std::string &soundName) const {
    try {
        PlaySound(resourceManager.getSound(soundName));
    } catch (...) {
        spdlog::error("Sound with the name {} was used, but it does not exist.", soundName);
    }
}

AudioService::AudioService() { InitAudioDevice(); }

AudioService::~AudioService() {
    resourceManager.unloadAllSounds();
    CloseAudioDevice();
}