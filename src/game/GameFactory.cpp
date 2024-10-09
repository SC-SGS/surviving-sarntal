//
// Created by Aleksis Vezenkov on 08.09.24.
//

#include "GameFactory.hpp"
#include "DevMode.hpp"

GameFactory::GameFactory(Camera2D &camera)
    : // Service
      configManager(ConfigManager::getInstance()),
      inputHandler(InputHandler::getInstance()),
      resourceManager(ResourceManager(configManager, true)),
      audioService(AudioService(resourceManager)),

      // Game
      gameConstants(configManager.getGameConstants()),
      terrain(gameConstants.hikerConstants, gameConstants.terrainConstants, resourceManager),
      // terrain(groundPoints, gameConstants.hikerConstants, gameConstants.terrainConstants, resourceManager),
      hiker({0.0, 0.0}, audioService, gameConstants.hikerConstants),
      monster(gameConstants),
      inventory(audioService, gameConstants.itemsConstants),
      world(terrain, hiker, inventory, monster, audioService, gameConstants),
      // Menu
      menuEngine(resourceManager),
      fullMenuRenderer(menuEngine),
      menuEventProcessor(menuEngine),
      // Renderer
      camera(camera),
      polygonRenderer(resourceManager),
      mountainRenderer(camera, gameConstants, resourceManager),
      entityRenderer(world, camera, gameConstants, resourceManager, polygonRenderer),
      hudRenderer(world, camera, gameConstants, resourceManager),
      renderer(
          world, resourceManager, camera, gameConstants, menuEngine, mountainRenderer, entityRenderer, hudRenderer),
      collisionRenderer(world, gameConstants, camera),
      // Spawner
      items(configManager.getItems()),
      itemSpawner(world, gameConstants, items),
      rockSpawner(world, gameConstants),
      spawner(terrain, rockSpawner, itemSpawner, world, gameConstants),
      // Physics
      accelerator(world, gameConstants),
      collisionDetector(world, gameConstants, collisionRenderer, configManager.isInDevMode()),
      collisionHandler(world, collisionDetector, audioService, renderer, gameConstants),
      destructor(world, entityRenderer, gameConstants),
      interpolator(world),
      positioner(world,
                 gameConstants.hikerConstants,
                 gameConstants.barriersConstants,
                 gameConstants.physicsConstants,
                 gameConstants.terrainConstants),
      gameEventProcessor(world, renderer, gameConstants, menuEngine, audioService),
      physicsEngine(world,
                    spawner,
                    gameConstants.physicsConstants,
                    gameEventProcessor,
                    accelerator,
                    positioner,
                    collisionDetector,
                    collisionHandler,
                    interpolator,
                    destructor),
      // Game
      game(world,
           renderer,
           fullMenuRenderer,
           menuEngine,
           menuEventProcessor,
           physicsEngine,
           audioService,
           inputHandler,
           gameConstants),
      devMode(world, renderer, physicsEngine, spawner, audioService, inputHandler, gameConstants, camera) {}

GameFactory::~GameFactory() = default;
Game GameFactory::buildGame() {
    this->terrain.initialize();
    this->monster.setXPosition(this->gameConstants.barriersConstants.monsterXRelativeToScreenWidth *
                                   (this->world.getMaxX() - this->world.getMinX()) +
                               this->world.getMinX());
    this->hiker.setPosition(this->getInitialHikerPosition());
    this->hiker.move({0.f, 0.f});
    return this->game;
}
DevMode GameFactory::buildDevMode() {
    this->world.setMinX(-10);
    this->world.setMaxX(30);
    this->devMode.init();
    this->monster.setXPosition(-20.f);
    return this->devMode;
}

Vector GameFactory::getInitialHikerPosition() {
    floatType hikerPositionX = this->gameConstants.hikerConstants.spawnXRelativeToScreenWidth *
                                   (this->world.getMaxX() - this->world.getMinX()) +
                               this->world.getMinX();
    floatType hikerPositionY = terrain.getMaxHeight(hikerPositionX);
    return {hikerPositionX, hikerPositionY};
}