//
// Created by Aleksis Vezenkov on 02.06.24.
//

#ifndef SURVIVING_SARNTAL_CONFIGMANAGER_HPP
#define SURVIVING_SARNTAL_CONFIGMANAGER_HPP

#include "Singleton.hpp"
#include <yaml-cpp/yaml.h>

class ConfigManager : public Singleton<ConfigManager> {
    friend class Singleton<ConfigManager>; // Allow Singleton to access the constructor??

  public:
    YAML::Node getTextures();
    YAML::Node getSounds();
    YAML::Node getItems();
    std::unordered_map<std::string, int> getLandmarks();

  private:
    ConfigManager();
    ~ConfigManager();
    YAML::Node config;
};

#endif // SURVIVING_SARNTAL_CONFIGMANAGER_HPP
