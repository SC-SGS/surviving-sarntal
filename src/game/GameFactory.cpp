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
      hiker(Hiker(this->getInitialHikerPosition(), audioService, gameConstants.hikerConstants)),
      monster(Monster(gameConstants.hikerConstants)),
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
      // Spawner
      items(configManager.getItems()),
      itemSpawner(world, gameConstants, items),
      rockSpawner(world, gameConstants),
      spawner(terrain, rockSpawner, itemSpawner, world, gameConstants),
      // Physics
      accelerator(world, gameConstants),
      collisionDetector(world, gameConstants, configManager.isInDevMode()),
      collisionHandler(world, collisionDetector, audioService, renderer, gameConstants),
      destructor(world, entityRenderer, gameConstants),
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
           gameConstants),
      devMode(world, renderer, physicsEngine, spawner, audioService, inputHandler, gameConstants, camera) {}

GameFactory::~GameFactory() = default;
Game GameFactory::buildGame() const { return this->game; }
DevMode GameFactory::buildDevMode() const { return this->devMode; }

Vector GameFactory::getInitialHikerPosition() {
    floatType hikerPositionX = 0.3f * (static_cast<float>(GetScreenWidth()) / graphics::UNIT_TO_PIXEL_RATIO);
    floatType hikerPositionY = terrain.getGroundHeight(hikerPositionX);
    return {hikerPositionX, hikerPositionY};
}
