//
// Created by Aleksis Vezenkov on 30.05.24.
//

#include "AudioService.hpp"

#include <mutex>

void AudioService::playSound(const std::string &soundName) const {
    try {
        PlaySound(resourceManager.getSound(soundName));
    } catch (...) {
        std::cout << "Sound name was used that does not exist" << std::endl;
    }
}

AudioService::AudioService() {
    InitAudioDevice();
    std::cout << "Audio Service initialized." << std::endl;
}

AudioService::~AudioService() {
    resourceManager.unloadAllSounds();
    CloseAudioDevice();
    std::cout << "Audio Service destructed." << std::endl;
}