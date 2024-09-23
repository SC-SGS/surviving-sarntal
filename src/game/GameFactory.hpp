//
// Created by Aleksis Vezenkov on 08.09.24.
//

#ifndef SURVIVING_SARNTAL_GAMEFACTORY_HPP
#define SURVIVING_SARNTAL_GAMEFACTORY_HPP

#include "Game.hpp"

class GameFactory {
  public:
    explicit GameFactory(Camera2D &camera);
    ~GameFactory();

    // API
    Game buildGame() const;

  private:
    // Services
    ConfigManager &configManager;
    InputHandler &inputHandler;
    ResourceManager resourceManager;
    AudioService audioService;

    // Game
    GameConstants gameConstants;
    Terrain terrain;
    Hiker hiker;
    Monster monster;
    Inventory inventory;
    World world;

    // Renderer
    PolygonRenderer polygonRenderer;
    MountainRenderer mountainRenderer;
    Renderer renderer;
    Camera2D &camera;

    // Menu
    MenuEngine menuEngine;
    FullMenuRenderer fullMenuRenderer;
    MenuEventProcessor menuEventProcessor;

    // Spawner
    std::unordered_map<ItemType, ItemDto> items;
    ItemSpawner itemSpawner;
    RockSpawner rockSpawner;
    Spawner spawner;

    // Physics
    Accelerator accelerator;
    CollisionDetector collisionDetector;
    CollisionHandler collisionHandler;
    Destructor destructor;
    Interpolator interpolator;
    Positioner positioner;
    GameEventProcessor gameEventProcessor;
    PhysicsEngine physicsEngine;

    // Game
    Game game;

    Vector getInitialHikerPosition();
};

#endif // SURVIVING_SARNTAL_GAMEFACTORY_HPP
