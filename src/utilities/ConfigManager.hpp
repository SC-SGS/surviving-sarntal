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
    bool musicShouldBePlayed();
    std::vector<Vector> getGroundPointsDevMode();

  private:
    ConfigManager();
    ~ConfigManager();

    // Configs locations
    const std::string CONFIG_LOCATION = "../../src/configuration.yaml";
    const std::string CONFIG_DEV_MODE_LOCATION = "../../src/game/configuration-dev-mode.yaml";

    // Attributes
    YAML::Node config;
    YAML::Node configDevMode;

    // Helper functions
    std::unordered_map<std::string, std::string> extractMap(std::string const &propertyName);
    YAML::Node loadConfiguration(const std::string &relativePath);
};

#endif // SURVIVING_SARNTAL_CONFIGMANAGER_HPP
