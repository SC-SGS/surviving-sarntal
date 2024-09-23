//
// Created by Aleksis Vezenkov on 02.06.24.
//

#include "ConfigManager.hpp"
#include "../game/GameProperties.hpp"
#include "YamlConversions.hpp"
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

std::unordered_map<std::string, std::string> ConfigManager::getSounds() { return extractMap("soundEffects"); }

std::unordered_map<std::string, std::string> ConfigManager::getTextures() { return extractMap("textures"); }

std::unordered_map<ItemType, ItemDto> ConfigManager::getItems() {
    std::unordered_map<ItemType, ItemDto> items;
    auto itemsConfig = config["items"];

    for (YAML::const_iterator it = itemsConfig.begin(); it != itemsConfig.end(); ++it) {
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

GameConstants ConfigManager::getGameConstants() { return config["gameConstants"].as<GameConstants>(); }

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