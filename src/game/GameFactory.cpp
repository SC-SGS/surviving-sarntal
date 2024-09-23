//
// Created by Aleksis Vezenkov on 08.09.24.
//

#include "GameFactory.hpp"

GameFactory::GameFactory(Camera2D &camera)
    : // Service
      configManager(ConfigManager::getInstance()),
      inputHandler(InputHandler::getInstance()),
      resourceManager(ResourceManager(configManager, true)),
      audioService(AudioService(resourceManager)),

      // Game
      gameConstants(configManager.getGameConstants()),
      terrain(gameConstants.hikerConstants, gameConstants.terrainConstants, resourceManager),
      hiker(Hiker(this->getInitialHikerPosition(), audioService, gameConstants.hikerConstants)),
      monster(Monster(gameConstants.hikerConstants)),
      inventory(audioService, gameConstants.itemsConstants),
      world(terrain, hiker, inventory, monster, audioService, gameConstants),
      // Renderer
      camera(camera),
      polygonRenderer(resourceManager),
      mountainRenderer(camera, gameConstants, resourceManager),
      renderer(world, resourceManager, camera, mountainRenderer, gameConstants, polygonRenderer),
      // Menu
      menuEngine(resourceManager),
      fullMenuRenderer(menuEngine),
      menuEventProcessor(menuEngine),
      // Spawner
      items(configManager.getItems()),
      itemSpawner(world, gameConstants, items),
      rockSpawner(world, gameConstants),
      spawner(terrain, rockSpawner, itemSpawner, world, gameConstants),
      // Physics
      accelerator(world, gameConstants),
      collisionDetector(world),
      collisionHandler(world, collisionDetector, audioService, renderer, gameConstants),
      destructor(world, renderer, gameConstants),
      interpolator(world),
      positioner(world,
                 gameConstants.hikerConstants,
                 gameConstants.barriersConstants,
                 gameConstants.physicsConstants,
                 gameConstants.terrainConstants),
      gameEventProcessor(world, renderer, gameConstants.hikerConstants, menuEngine),
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
           gameConstants) {}

GameFactory::~GameFactory() = default;
Game GameFactory::buildGame() const { return this->game; }

Vector GameFactory::getInitialHikerPosition() {
    floatType hikerPositionX = 0.3 * graphics::SCREEN_WIDTH_IN_METER;
    floatType hikerPositionY = terrain.getGroundHeight(hikerPositionX);
    return {hikerPositionX, hikerPositionY};
}