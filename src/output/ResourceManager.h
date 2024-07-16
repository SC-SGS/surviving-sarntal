//
// Created by six on 5/13/24.
//

#ifndef SURVIVING_SARNTAL_RESOURCEMANAGER_H
#define SURVIVING_SARNTAL_RESOURCEMANAGER_H

#include "../utilities/ConfigManager.hpp"
#include "../utilities/Singleton.hpp"
#include "raylib.h"
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

class ResourceManager {

  private:
    virtual void loadMusic();
    virtual void loadTextures();

    ConfigManager &config;

    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> soundEffects;
    std::unordered_map<std::string, Music> musics;

  public:
    ~ResourceManager() = default;
    void initialize();
    explicit ResourceManager(ConfigManager &configManager);
    void loadTexture(const std::string &name, const std::string &path);
    Texture2D getTexture(const std::string &name);
    void loadSound(const std::string &name, const std::string &path);
    Sound getSound(const std::string &name);
    void loadMusic(const std::string &name, const std::string &path);
    Music getMusic(const std::string &name);
    void unloadAllSounds() const;
};
#endif // SURVIVING_SARNTAL_RESOURCEMANAGER_H
