//
// Created by bjoern on 5/31/24.
//

#include "ResourceManager.h"

#include <mutex>

ResourceManager::ResourceManager() {
    this->loadTextures();
    this->loadMusic();
}

void ResourceManager::loadTextures() {
    auto configTextures = config.getTextures();

    for (YAML::const_iterator it = configTextures.begin(); it != configTextures.end(); ++it) {
        auto textureName = it->first.as<std::string>();
        auto textureLocation = it->second.as<std::string>();
        this->textures[textureName] = LoadTexture(textureLocation.c_str());
    }
}

void ResourceManager::loadMusic() {
    auto configSounds = config.getSounds();

    for (YAML::const_iterator it = configSounds.begin(); it != configSounds.end(); ++it) {
        auto soundName = it->first.as<std::string>();
        auto soundLocation = it->second.as<std::string>();
        this->soundEffects[soundName] = LoadSound(soundLocation.c_str());
    }
}

ResourceManager::~ResourceManager() = default;

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