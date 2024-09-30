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
    virtual void loadSounds();
    virtual void loadTextures();

    ConfigManager &config;

    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> soundEffects;
    std::unordered_map<std::string, Music> musics;

  public:
    ~ResourceManager() = default;

    /**
     * This constructor does not initialize the assets needed for the game.
     * We need this, so that we can mock the class without using Raylib to load the assets.
     * If you want to initialize the assets you need to use another constructor or call the init() function afterwards.
     */
    explicit ResourceManager(ConfigManager &configManager);
    ResourceManager(ConfigManager &configManager, bool initOnBuild);
    void loadTexture(const std::string &name, const std::string &path);
    Texture2D getTexture(const std::string &name);
    void loadSound(const std::string &name, const std::string &path);
    Sound getSound(const std::string &name);
    void loadMusic(const std::string &name, const std::string &path);
    Music getMusic(const std::string &name);
    void unloadAllSounds() const;
    void init();
};
#endif // SURVIVING_SARNTAL_RESOURCEMANAGER_H
