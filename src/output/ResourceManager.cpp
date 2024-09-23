//
// Created by bjoern on 5/31/24.
//

#include "ResourceManager.h"

#include <mutex>

ResourceManager::ResourceManager(ConfigManager &configManager) : config(configManager) {}

ResourceManager::ResourceManager(ConfigManager &configManager, bool initOnBuild) : config(configManager) {
    if (initOnBuild) {
        this->init();
    }
}

void ResourceManager::init() {
    this->loadTextures();
    this->loadMusic();
}

void ResourceManager::loadTextures() {
    auto configTextures = config.getTextures();

    for (auto &texture : configTextures) {
        auto textureName = texture.first;
        auto textureLocation = texture.second;
        this->textures[textureName] = LoadTexture(textureLocation.c_str());
    }
}

void ResourceManager::loadMusic() {
    std::unordered_map<std::string, std::string> configSounds = config.getSounds();

    for (auto &configSound : configSounds) {
        auto soundName = configSound.first;
        auto soundLocation = configSound.second;
        this->soundEffects[soundName] = LoadSound(soundLocation.c_str());
    }
}

void ResourceManager::loadTexture(const std::string &name, const std::string &path) {
    if (textures.find(name) != textures.end()) {
        throw std::runtime_error("Texture with name " + name + " already exists");
    } else {
        textures[name] = LoadTexture(path.c_str());
    }
}

Texture2D ResourceManager::getTexture(const std::string &name) {
    if (textures.find(name) == textures.end()) {
        throw std::runtime_error("Texture with name " + name + " does not exist");
    }
    return textures[name];
}

void ResourceManager::loadSound(const std::string &name, const std::string &path) {
    if (soundEffects.find(name) != soundEffects.end()) {
        throw std::runtime_error("Sound with name " + name + " already exists");
    } else {
        soundEffects[name] = LoadSound(path.c_str());
    }
}

Sound ResourceManager::getSound(const std::string &name) {
    if (soundEffects.find(name) == soundEffects.end()) {
        throw std::runtime_error("Sound with name " + name + " does not exist");
    }
    return soundEffects[name];
}

void ResourceManager::loadMusic(const std::string &name, const std::string &path) {
    if (musics.find(name) != musics.end()) {
        throw std::runtime_error("Music with name " + name + " already exists");
    }
    musics[name] = LoadMusicStream(path.c_str());
}

Music ResourceManager::getMusic(const std::string &name) {
    if (musics.find(name) == musics.end()) {
        throw std::runtime_error("Music with name " + name + " does not exist");
    }
    return musics[name];
}

void ResourceManager::unloadAllSounds() const {
    for (auto &entry : soundEffects) {
        UnloadSound(entry.second);
    }
}