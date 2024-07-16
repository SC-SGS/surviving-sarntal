//
// Created by Aleksis Vezenkov on 31.05.24.
//

#ifndef SURVIVING_SARNTAL_AUDIOSERVICE_HPP
#define SURVIVING_SARNTAL_AUDIOSERVICE_HPP

#include "../ResourceManager.h"
#include <string>

class AudioService {

  public:
    ~AudioService();
    virtual void playSound(const std::string &soundName) const;
    explicit AudioService(ResourceManager &resourceManager);

  private:
    ResourceManager &resourceManager;
};

#endif // SURVIVING_SARNTAL_AUDIOSERVICE_HPP