#include "game/Game.hpp"
#include "spdlog/fmt/bundled/chrono.h"
#include "spdlog/spdlog.h"

// TODO extract initialization to game
int main(int argc, char *argv[]) { // NOLINT [readability-function-size,-warnings-as-errors]
    spdlog::set_level(spdlog::level::info);
    InitWindow(graphics::SCREEN_WIDTH, graphics::SCREEN_HEIGHT, "Surviving Sarntal");

    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);
    SDL_Init(SDL_INIT_GAMECONTROLLER);

    // Init Services todo rename the other managers / handlers to one of these: service or manager or handler
    ConfigManager &configManager = ConfigManager::getInstance();
    InputHandler &inputHandler = InputHandler::getInstance();
    ResourceManager resourceManager(configManager);
    resourceManager.initialize();
    AudioService audioService(resourceManager);

    // Init game world
    Mountain mountain = Mountain(configManager.getGameConstants().mountainConstants);
    floatType hikerPositionX = 0.8 * graphics::SCREEN_WIDTH;
    Hiker hiker(Vector{hikerPositionX, mountain.calculateYPos(hikerPositionX)}, audioService,
                configManager.getGameConstants().hikerConstants);
    Monster monster(configManager.getGameConstants().hikerConstants);
    Inventory inventory(audioService, configManager.getGameConstants().itemsConstants);
    World world(mountain, hiker, inventory, monster, audioService, configManager.getGameConstants());

    PolygonRenderer polygonRenderer(resourceManager);
    Camera2D camera = {0};

    // Init renderer
    MountainRenderer mountainRenderer(camera, configManager.getGameConstants());
    Renderer renderer(world, resourceManager, camera, mountainRenderer, configManager.getGameConstants(),
                      polygonRenderer);

    // Init physics
    Accelerator accelerator(world, configManager.getGameConstants());
    CollisionDetector collisionDetector(world);
    CollisionHandler collisionHandler(world, collisionDetector, audioService, renderer,
                                      configManager.getGameConstants());
    Destructor destructor(world, renderer, configManager.getGameConstants());
    Interpolator interpolator(world);
    Positioner positioner(world, configManager.getGameConstants().hikerConstants,
                          configManager.getGameConstants().barriersConstants);
    EventProcessor eventProcessor(world, renderer, configManager.getGameConstants().hikerConstants);
    auto items = configManager.getItems();
    ItemSpawner itemSpawner(world, configManager.getGameConstants(), items);
    RockSpawner rockSpawner(world, configManager.getGameConstants());
    Spawner spawner(mountain, rockSpawner, itemSpawner, world, configManager.getGameConstants());

    PhysicsEngine physicsEngine(world, spawner, configManager.getGameConstants().physicsConstants, eventProcessor,
                                accelerator, positioner, collisionDetector, collisionHandler, interpolator, destructor);

    // Init game
    Game game(world, renderer, physicsEngine, audioService, inputHandler, configManager.getGameConstants());

    game.run();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}