//
// Created by Aleksis Vezenkov on 30.05.24.
//

#include "AudioService.hpp"

#include <mutex>

void AudioService::playSound(const std::string &soundName) const {
    try {
        PlaySound(resourceManager.getSound(soundName));
        spdlog::info("Playing a sound with name: {}", soundName);
    } catch (...) {
        spdlog::error("Sound with the name {} was used, but it does not exist.", soundName);
    }
}
void AudioService::playSong(const std::string &songName, const bool loop, const floatType vol) {
    try {
        Music music = resourceManager.getMusic(songName);
        music.looping = loop;
        PlayMusicStream(music);
        SetMusicVolume(music, vol);
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

void AudioService::playMovingSound(const floatType hikerSpeed, const floatType maxHikerSpeed) {
    try {
        // Get the sound from the resource manager
        const Sound sound = resourceManager.getSound("moving");
        const floatType speed = fmaxf(0.5, fabsf(hikerSpeed) / (maxHikerSpeed / 2.f));

        // Check if the sound is already playing
        if (!this->movementSoundPlaying || GetTime() - this->lastStartTimeMovementSound > 0.5) {
            if (!IsSoundPlaying(sound)) {
                // Play the sound if it's not already playing
                PlaySound(sound);
                this->movementSoundPlaying = true;
                this->lastStartTimeMovementSound = static_cast<floatType>(GetTime());

                // Log the action
                spdlog::info("Playing a sound with name: moving at speed: {}", speed);
            }
        }

        // Set the playback speed (pitch)
        SetSoundPitch(sound, speed);

        // Log the action
        spdlog::debug("Playing a sound with name: moving at speed: {}", speed);
    } catch (...) {
        // Handle any error (e.g., sound file not found) and log it
        spdlog::error("Sound with the name moving was used, but it does not exist.");
    }
}

void AudioService::interruptMovingSound() {
    this->interruptSound("moving");
    this->movementSoundPlaying = false;
    this->lastStartTimeMovementSound = 0.0f;
}

void AudioService::playSoundWithSpeedIfNotAlreadyPlaying(const std::string &soundName, float speed) const {
    try {
        // Get the sound from the resource manager
        const Sound sound = resourceManager.getSound(soundName);

        // Check if the sound is already playing
        if (!IsSoundPlaying(sound)) {
            // Play the sound if it's not already playing
            PlaySound(sound);

            // Log the action
            spdlog::info("Playing a sound with name: {} at speed: {}", soundName, speed);

            // Set the playback speed (pitch)
            SetSoundPitch(sound, speed);
        }

        // Log the action
        spdlog::debug("Playing a sound with name: {} at speed: {}", soundName, speed);
    } catch (...) {
        // Handle any error (e.g., sound file not found) and log it
        spdlog::error("Sound with the name {} was used, but it does not exist.", soundName);
    }
}

void AudioService::interruptSound(const std::string &soundName) const {
    try {
        // Get the sound from the resource manager
        Sound sound = resourceManager.getSound(soundName);

        // Check if the sound is playing
        if (IsSoundPlaying(sound)) {
            StopSound(sound); // Stop the currently playing sound

            // Log the action
            spdlog::info("Interrupting sound: {}", soundName);
        }
    } catch (...) {
        // Handle any error (e.g., sound file not found) and log it
        spdlog::error("Sound with the name {} was used, but it does not exist.", soundName);
    }
}

AudioService::AudioService(ResourceManager &resourceManager) : resourceManager(resourceManager){};

AudioService::~AudioService() { resourceManager.unloadAllSounds(); }