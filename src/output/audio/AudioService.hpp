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
    virtual void playSong(const std::string &songName, bool loop);
    void updateMusicStream();

  private:
    ResourceManager &resourceManager;
    std::list<Music> musicList;
};

#endif // SURVIVING_SARNTAL_AUDIOSERVICE_HPP