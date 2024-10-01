//
// Created by Aleksis Vezenkov on 02.06.24.
//

#ifndef SURVIVING_SARNTAL_CONFIGMANAGER_HPP
#define SURVIVING_SARNTAL_CONFIGMANAGER_HPP

#include "../entities/Item.hpp"
#include "../game/GameProperties.hpp"
#include "Singleton.hpp"
#include <yaml-cpp/yaml.h>

class ConfigManager : public Singleton<ConfigManager> {
    friend class Singleton<ConfigManager>;

  public:
    std::unordered_map<std::string, std::string> getTextures();
    std::unordered_map<std::string, std::string> getSounds();
    std::unordered_map<std::string, std::string> getMusic();
    std::unordered_map<ItemType, ItemDto> getItems();
    std::unordered_map<std::string, int> getLandmarks();
    bool getFullscreen();

    GameConstants getGameConstants();
    bool isInDevMode();

  private:
    ConfigManager();
    ~ConfigManager();

    // Attributes
    YAML::Node config;

    // Helper functions
    std::unordered_map<std::string, std::string> extractMap(std::string const &propertyName);
};

#endif // SURVIVING_SARNTAL_CONFIGMANAGER_HPP
