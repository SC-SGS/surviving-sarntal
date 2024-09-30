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
void AudioService::playSong(const std::string &songName, bool loop) {
    try {
        Music music = resourceManager.getMusic(songName);
        music.looping = loop;
        PlayMusicStream(music);
        musicList.insert(musicList.begin(), music);
        spdlog::debug("Playing song with name: {}", songName);
    } catch (...) {
        spdlog::error("Song with the name {} was used, but it does not exist.", songName);
    }
}

void AudioService::updateMusicStream() {
    for (auto it = musicList.begin(); it != musicList.end();) {
        if (IsAudioStreamPlaying(it->stream)) {
            UpdateMusicStream(*it);
            ++it;
        } else {
            it = musicList.erase(it);
        }
    }
}

AudioService::AudioService(ResourceManager &resourceManager) : resourceManager(resourceManager){};

AudioService::~AudioService() { resourceManager.unloadAllSounds(); }