#include "game/Game.hpp"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) { // NOLINT [readability-function-size,-warnings-as-errors]
    spdlog::set_level(spdlog::level::info);
    InitWindow(graphics::SCREEN_WIDTH, graphics::SCREEN_HEIGHT, "Surviving Sarntal");

    InputHandler &inputHandler = InputHandler::getInstance();
    ResourceManager resourceManager(ConfigManager::getInstance());
    AudioService audioService(resourceManager);

    Mountain mountain;
    floatType hikerPositionX = 0.8 * graphics::SCREEN_WIDTH;
    Hiker hiker(Vector{hikerPositionX, mountain.getYPosFromX(hikerPositionX)}, audioService);
    Monster monster;
    Inventory inventory(audioService);
    World world(mountain, hiker, inventory, monster, audioService);

    Renderer renderer(world, resourceManager);

    Accelerator accelerator(world);
    CollisionDetector collisionDetector(world);
    CollisionHandler collisionHandler(world, collisionDetector, audioService, renderer);
    Destructor destructor(world);
    Interpolator interpolator(world);
    Positioner positioner(world);
    EventProcessor eventProcessor(world, renderer);

    ItemSpawner itemSpawner(world);
    RockSpawner rockSpawner(world);
    MountainGenerator mountainGenerator(world);
    Spawner spawner(mountainGenerator, rockSpawner, itemSpawner);

    PhysicsEngine physicsEngine(world, spawner, eventProcessor, accelerator, positioner, collisionDetector,
                                collisionHandler, interpolator, destructor);

    Game game(world, renderer, physicsEngine, audioService, inputHandler);

    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);
    SDL_Init(SDL_INIT_GAMECONTROLLER);

    game.run();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}