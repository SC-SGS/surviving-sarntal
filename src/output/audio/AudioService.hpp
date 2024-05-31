//
// Created by Aleksis Vezenkov on 31.05.24.
//

#ifndef SURVIVING_SARNTAL_AUDIOSERVICE_HPP
#define SURVIVING_SARNTAL_AUDIOSERVICE_HPP

#include "../ResourceManager.h"
#include "raylib.h"
#include <iostream>
#include <string>

class AudioService {
  public:
    static AudioService &getInstance();
    AudioService(const AudioService &) = delete;
    void operator=(const AudioService &) = delete;
    void playSound(const std::string &soundName) const;

  private:
    AudioService();
    ~AudioService();
    ResourceManager &resourceManager = ResourceManager::getInstance();
};

#endif // SURVIVING_SARNTAL_AUDIOSERVICE_HPP