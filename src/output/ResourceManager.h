//
// Created by six on 5/13/24.
//

#ifndef SURVIVING_SARNTAL_RESOURCEMANAGER_H
#define SURVIVING_SARNTAL_RESOURCEMANAGER_H

#include "raylib.h"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class ResourceManager {
  private:
    ResourceManager() = default;
    std::unordered_map<std::string, Texture2D> textures = { // TODO take this from the configuration see issue #30
        {"crouch", LoadTexture("../assets/texture/player_duck.png")},
        {"jump", LoadTexture("../assets/texture/player_jump.png")},
        {"walk", LoadTexture("../assets/texture/player_walk.png")},
        {"kaiserschmarrn", LoadTexture("../assets/texture/kaiserschmarrn.png")},
        {"coin", LoadTexture("../assets/texture/coin.png")},
        {"duck", LoadTexture("../assets/texture/duck.png")},
        {"rock", LoadTexture("../assets/texture/stone.png")},
        {"helicopter", LoadTexture("../assets/texture/helicopter.png")},
        {"midground", LoadTexture("../assets/layers/glacial_mountains.png")},
        {"foreground", LoadTexture("../assets/layers/clouds_mg_1.png")}};
    std::unordered_map<std::string, Sound> soundEffects = {
        {"background_music", LoadSound("../assets/audio/background_music.mp3")},
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
        {"proper_duck", LoadSound("../assets/audio/proper_duck.wav")},
        {"proper_jump", LoadSound("../assets/audio/proper_jump.wav")},
        {"proper_rock_smash", LoadSound("../assets/audio/proper_rock_smash.wav")},
        {"shutdown", LoadSound("../assets/audio/shutdown.wav")}

    };
    std::unordered_map<std::string, Music> musics;

  public:
    static ResourceManager &getInstance() {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(ResourceManager const &) = delete;
    void operator=(ResourceManager const &) = delete;

    void loadTexture(const std::string &name, const std::string &path) {
        if (textures.find(name) != textures.end()) {
            throw std::runtime_error("Texture with name " + name + " already exists");
        } else {
            textures[name] = LoadTexture(path.c_str());
        }
    }

    Texture2D getTexture(const std::string &name) {
        if (textures.find(name) == textures.end()) {
            throw std::runtime_error("Texture with name " + name + " does not exist");
        }
        return textures[name];
    }

    void loadSound(const std::string &name, const std::string &path) {
        if (soundEffects.find(name) != soundEffects.end()) {
            throw std::runtime_error("Sound with name " + name + " already exists");
        } else {
            soundEffects[name] = LoadSound(path.c_str());
        }
    }

    Sound getSound(const std::string &name) {
        if (soundEffects.find(name) == soundEffects.end()) {
            throw std::runtime_error("Sound with name " + name + " does not exist");
        }
        return soundEffects[name];
    }

    void loadMusic(const std::string &name, const std::string &path) {
        if (musics.find(name) != musics.end()) {
            throw std::runtime_error("Music with name " + name + " already exists");
        }
        musics[name] = LoadMusicStream(path.c_str());
    }

    Music getMusic(const std::string &name) {
        if (musics.find(name) == musics.end()) {
            throw std::runtime_error("Music with name " + name + " does not exist");
        }
        return musics[name];
    }

    void unloadAllSounds() {
        for (auto &entry : soundEffects) {
            UnloadSound(entry.second);
        }
    }
};
#endif // SURVIVING_SARNTAL_RESOURCEMANAGER_H
