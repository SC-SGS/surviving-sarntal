//
// Created by Aleksis Vezenkov on 02.06.24.
//

#include "ConfigManager.hpp"
#include "iostream"

void ConfigManager::testPublicFunction() {
    std::cout << "test function of the config manager" << std::endl;
    if (config.IsDefined()) {
        std::cout << "Config is defined" << std::endl;
    };

    auto textures = config["textures"];

    for (YAML::const_iterator it = textures.begin(); it != textures.end(); ++it) {
        std::cout << "Texture name " << it->first.as<std::string>() << " is mapped to location "
                  << it->second.as<std::string>() << "\n";
    }
}

ConfigManager::ConfigManager() {
    std::cout << "ConfigManager constructed" << std::endl;
    config = YAML::LoadFile("../src/configuration.yaml");
}
ConfigManager::~ConfigManager() { std::cout << "ConfigManager destructed" << std::endl; }

std::string ConfigManager::textureLocation(const std::string &textureName) {
    if (config["textures"][textureName]) {
        return config["textures"][textureName].as<std::string>();
    } else {
        return "";
    }
}

YAML::Node ConfigManager::getTextures() { return config["textures"]; }
YAML::Node ConfigManager::getConfig() { return config; }
