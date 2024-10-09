//
// Created by Aleksis Vezenkov on 02.06.24.
//

#include "ConfigManager.hpp"
#include "../game/GameProperties.hpp"
#include "YamlConversions.hpp"
#include "iostream"

ConfigManager::ConfigManager() {
    config = this->loadConfiguration(this->CONFIG_LOCATION);
    configDevMode = this->loadConfiguration(this->CONFIG_DEV_MODE_LOCATION);
}

YAML::Node ConfigManager::loadConfiguration(const std::string &relativePath) {
    YAML::Node configurationNode;
    try {
        configurationNode = YAML::LoadFile(relativePath);
    } catch (...) {
        spdlog::critical("Configuration file could not be loaded. Check if file with relative location to the "
                         "ConfigManager.cpp: {} exists.",
                         relativePath);
        throw std::runtime_error("Something went wrong when loading the configuration.");
    }
    spdlog::info("Configuration file at: {} was loaded successfully.", relativePath);
    return configurationNode;
}

ConfigManager::~ConfigManager() = default;

std::unordered_map<std::string, std::string> ConfigManager::getSounds() { return extractMap("soundEffects"); }
std::unordered_map<std::string, std::string> ConfigManager::getMusic() { return extractMap("music"); }

std::unordered_map<std::string, std::string> ConfigManager::getTextures() { return extractMap("textures"); }

std::unordered_map<ItemType, ItemDto> ConfigManager::getItems() {
    std::unordered_map<ItemType, ItemDto> items;
    const auto &itemsConfig = config["items"];

    for (auto it = itemsConfig.begin(); it != itemsConfig.end(); ++it) {
        ItemDto dto;

        dto.name = it->first.as<std::string>();
        dto.itemType = ItemType(it->second["typeId"].as<int>());
        dto.autoCollect = it->second["autoCollect"].as<bool>();
        dto.useOnPickup = it->second["useOnPickUp"].as<bool>();
        dto.dropOnUse = it->second["dropOnUse"].as<bool>();
        dto.spawnWeight = it->second["spawnWeight"].as<int>();

        items[dto.itemType] = dto;
    }

    return items;
}

std::unordered_map<std::string, int> ConfigManager::getLandmarks() {
    std::unordered_map<std::string, int> landmarks;
    if (config["landmarks"]) {
        for (const auto &landmark : config["landmarks"]) {
            auto name = landmark["name"].as<std::string>();
            int height = landmark["height"].as<int>();
            landmarks[name] = height;
        }
    }
    return landmarks;
}

bool ConfigManager::getFullscreen() { return config["fullscreen"].as<bool>(); }

GameConstants ConfigManager::getGameConstants() {
    if (this->isInDevMode()) {
        return configDevMode["gameConstants"].as<GameConstants>();
    } else {
        return config["gameConstants"].as<GameConstants>();
    }
}

std::unordered_map<std::string, std::string> ConfigManager::extractMap(std::string const &propertyName) {
    const YAML::Node propertyConfig = config[propertyName];
    std::unordered_map<std::string, std::string> map;

    for (YAML::const_iterator it = propertyConfig.begin(); it != propertyConfig.end(); ++it) {
        auto soundName = it->first.as<std::string>();
        auto soundLocation = it->second.as<std::string>();
        map[soundName] = soundLocation;
    }

    return map;
}

bool ConfigManager::isInDevMode() { return this->config["run-dev-mode"].as<bool>(); }
bool ConfigManager::musicShouldBePlayed() { return this->config["music-should-be-played"].as<bool>(); }

std::vector<Vector> ConfigManager::getGroundPointsDevMode() {
    return this->config["ground-points"].as<std::vector<Vector>>();
}