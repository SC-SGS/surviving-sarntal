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
    this->textures = {// TODO take this from the configuration see issue #30
                      {"crouch", LoadTexture("../assets/texture/player_duck.png")},
                      {"jump", LoadTexture("../assets/texture/player_jump.png")},
                      {"walk", LoadTexture("../assets/texture/player_walk.png")},
                      {"Kaiserschmarrn", LoadTexture("../assets/texture/kaiserschmarrn.png")},
                      {"Coin", LoadTexture("../assets/texture/coin.png")},
                      {"Duck", LoadTexture("../assets/texture/duck.png")},
                      {"rock", LoadTexture("../assets/texture/stone.png")},
                      {"helicopter", LoadTexture("../assets/texture/helicopter.png")},
                      {"monster", LoadTexture("../assets/texture/killbar.png")},
                      {"midground", LoadTexture("../assets/layers/glacial_mountains.png")},
                      {"foreground", LoadTexture("../assets/layers/clouds_mg_1.png")},
                      {"mountain", LoadTexture("../assets/layers/mountain.png")}};
}

void ResourceManager::loadMusic() {
    this->soundEffects = {{"background_music", LoadSound("../assets/audio/background_music.mp3")},
                          {"background_music2", LoadSound("../assets/audio/background_music2.wav")},
                          {"boom", LoadSound("../assets/audio/boom.wav")},
                          {"cool_sound", LoadSound("../assets/audio/cool_sound.mp3")},
                          {"duck", LoadSound("../assets/audio/duck.wav")},
                          {"dudum", LoadSound("../assets/audio/dudum.wav")},
                          {"error", LoadSound("../assets/audio/error.wav")},
                          {"fart", LoadSound("../assets/audio/fart.wav")},
                          {"game_over", LoadSound("../assets/audio/game_over.wav")},
                          {"gameover", LoadSound("../assets/audio/gameover.wav")},
                          {"jump", LoadSound("../assets/audio/jump.wav")},
                          {"mepmep", LoadSound("../assets/audio/mepmep.wav")},
                          {"ohje", LoadSound("../assets/audio/ohje.wav")},
                          {"oof", LoadSound("../assets/audio/oof.wav")},
                          {"pickup", LoadSound("../assets/audio/pickup.wav")},
                          {"pipe", LoadSound("../assets/audio/pipe.wav")},
                          {"proper_coin", LoadSound("../assets/audio/proper_coin.wav")},
                          {"proper_crouch", LoadSound("../assets/audio/proper_crouch.wav")},
                          {"proper_jump", LoadSound("../assets/audio/proper_jump.wav")},
                          {"proper_rock_smash", LoadSound("../assets/audio/proper_rock_smash.wav")},
                          {"shutdown", LoadSound("../assets/audio/shutdown.wav")}};
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