//
// Created by Aleksis Vezenkov on 31.05.24.
//

#ifndef SURVIVING_SARNTAL_AUDIOSERVICE_HPP
#define SURVIVING_SARNTAL_AUDIOSERVICE_HPP

#include "../ResourceManager.h"
#include <string>

class AudioService {

  public:
    explicit AudioService(ResourceManager &resourceManager);
    ~AudioService();
    virtual void playSound(const std::string &soundName) const;
    virtual void playSong(const std::string &songName, bool loop, floatType vol);
    void updateMusicStream();
    void playMovingSound(floatType hikerSpeed, floatType maxHikerSpeed);
    void interruptMovingSound();
    void playSoundWithSpeedIfNotAlreadyPlaying(const std::string &soundName, float speed) const;
    virtual void interruptSound(const std::string &soundName) const;

  private:
    ResourceManager &resourceManager;
    std::list<Music> musicList;
    bool movementSoundPlaying = false;
    floatType lastStartTimeMovementSound = 0.0f;
};

#endif // SURVIVING_SARNTAL_AUDIOSERVICE_HPP