#include "game/Game.hpp"
#include "spawner/PolygonGenerator.h"
#include "spdlog/fmt/bundled/chrono.h"
#include "spdlog/spdlog.h"

void testIntersection();
void renderTerrainTest(Terrain terrain);
Vector2 transformCoordinatesTesting(floatType minY, floatType maxY, Vector position);

// TODO extract initialization to game
int main(int argc, char *argv[]) { // NOLINT [readability-function-size,-warnings-as-errors]
    spdlog::set_level(spdlog::level::debug);
    testIntersection();
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
    GameConstants gameConstants = configManager.getGameConstants();
    auto terrain = Terrain(gameConstants.hikerConstants, gameConstants.terrainConstants);
    floatType hikerPositionX = 0.3 * (graphics::SCREEN_WIDTH / graphics::UNIT_TO_PIXEL_RATIO);
    floatType hikerPositionY = terrain.getGroundHeight(hikerPositionX);
    Hiker hiker(Vector{hikerPositionX, hikerPositionY}, audioService, gameConstants.hikerConstants);
    Monster monster(gameConstants.hikerConstants);
    Inventory inventory(audioService, gameConstants.itemsConstants);
    World world(terrain, hiker, inventory, monster, audioService, gameConstants);

    PolygonRenderer polygonRenderer(resourceManager);
    Camera2D camera = {0};

    // Init renderer
    MountainRenderer mountainRenderer(camera, gameConstants, resourceManager);
    Renderer renderer(world, resourceManager, camera, mountainRenderer, gameConstants, polygonRenderer);

    // Init physics
    Accelerator accelerator(world, gameConstants);
    CollisionDetector collisionDetector(world);
    CollisionHandler collisionHandler(world, collisionDetector, audioService, renderer, gameConstants);
    Destructor destructor(world, renderer, gameConstants);
    Interpolator interpolator(world);
    Positioner positioner(world, gameConstants.hikerConstants, gameConstants.barriersConstants);
    EventProcessor eventProcessor(world, renderer, gameConstants.hikerConstants);
    auto items = configManager.getItems();
    ItemSpawner itemSpawner(world, gameConstants, items);
    RockSpawner rockSpawner(world, gameConstants);
    Spawner spawner(terrain, rockSpawner, itemSpawner, world, gameConstants);

    PhysicsEngine physicsEngine(world, spawner, gameConstants.physicsConstants, eventProcessor, accelerator, positioner,
                                collisionDetector, collisionHandler, interpolator, destructor);

    // Init game
    Game game(world, renderer, physicsEngine, audioService, inputHandler, gameConstants);

    game.run();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void testIntersection() {
    Vector start1 = {1, 1};
    Vector end1 = {4, 4};
    Vector start2 = {2, 4};
    Vector end2 = {3.5, -0.5};
    Vector intersection = {2.5, 2.5};
    Line line1 = {start1, end1};
    Line line2 = {start2, end2};

    std::optional<Intersection> computedIntersection = line1.calculateIntersection(line2);

    // spdlog::debug("({}, {})", computedIntersection.x, computedIntersection.y);

    assert(computedIntersection.value().intersection == intersection);

    start1 = {0, -1};
    end1 = {0, 1};
    start2 = {1, 0};
    end2 = {-1, 0};
    intersection = {0, 0};
    line1 = {start1, end1};
    line2 = {start2, end2};

    computedIntersection = line1.calculateIntersection(line2);

    assert(computedIntersection.value().intersection == intersection);
}

Vector2 transformCoordinatesTesting(floatType minY, floatType maxY, Vector position) {
    return {position.x, minY + maxY - position.y};
}