//
// Created by Aleksis Vezenkov on 30.05.24.
//

#include "AudioService.hpp"

AudioService &AudioService::getInstance() {
    static AudioService instance;
    return instance;
}
void AudioService::playSound(const std::string &soundName) const { PlaySound(resourceManager.getSound(soundName)); }

AudioService::AudioService() {
    InitAudioDevice();
    std::cout << "Audio device initialized." << std::endl;
}

AudioService::~AudioService() {
    resourceManager.unloadAllSounds();
    CloseAudioDevice();
    std::cout << "Audio device closed." << std::endl;
}