//
// Created by Aleksis Vezenkov on 02.06.24.
//

#include "ConfigManager.hpp"
#include "iostream"

ConfigManager::ConfigManager() {
    std::string configLocation = "../../src/configuration.yaml";

    try {
        config = YAML::LoadFile(configLocation);
    } catch (...) {
        spdlog::critical("Configuration file could not be loaded. Check if file with relative location: {} exists.",
                         configLocation);
        throw std::runtime_error("Something went wrong when loading the configuration.");
    }
    spdlog::info("Configuration was loaded successfully.");
}
ConfigManager::~ConfigManager() = default;

YAML::Node ConfigManager::getSounds() { return config["soundEffects"]; }
YAML::Node ConfigManager::getTextures() { return config["textures"]; }
YAML::Node ConfigManager::getItems() { return config["items"]; }
