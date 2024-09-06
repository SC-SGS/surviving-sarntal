#include "game/Game.hpp"
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
    // Mountain mountain = Mountain(configManager.getGameConstants().mountainConstants);
    auto terrain =
        Terrain(configManager.getGameConstants().hikerConstants, configManager.getGameConstants().terrainConstants);
    floatType hikerPositionX = 0.3 * (graphics::SCREEN_WIDTH / graphics::UNIT_TO_PIXEL_RATIO);
    floatType hikerPositionY = terrain.getGroundHeight(hikerPositionX);
    Hiker hiker(Vector{hikerPositionX, hikerPositionY}, audioService, configManager.getGameConstants().hikerConstants);
    Monster monster(configManager.getGameConstants().hikerConstants);
    Inventory inventory(audioService, configManager.getGameConstants().itemsConstants);
    World world(terrain, hiker, inventory, monster, audioService, configManager.getGameConstants());

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
    Spawner spawner(terrain, rockSpawner, itemSpawner, world, configManager.getGameConstants());

    PhysicsEngine physicsEngine(world, spawner, configManager.getGameConstants().physicsConstants, eventProcessor,
                                accelerator, positioner, collisionDetector, collisionHandler, interpolator, destructor);

    // Init game
    Game game(world, renderer, physicsEngine, audioService, inputHandler, configManager.getGameConstants());

    // renderTerrainTest(terrain);

    game.run();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

// NOLINTBEGIN
void renderTerrainTest(Terrain terrain) {
    const floatType leftBorder = terrain.getLeftBorder() - 50.0;
    const floatType rightBorder = terrain.getRightBorder() + 50.0f;
    const floatType lowerBorder = terrain.getMinHeight() - 50;
    const floatType upperBorder = terrain.getMaxHeight() + 50;

    // Calculate the world width and height
    const floatType worldWidth = rightBorder - leftBorder;
    const floatType worldHeight = upperBorder - lowerBorder;

    // Calculate the zoom level so that the world width and height fits the screen width
    const auto screenWidth = static_cast<floatType>(GetScreenWidth());
    const auto screenHeight = static_cast<floatType>(GetScreenHeight());
    const floatType widthZoom = screenWidth / worldWidth;
    const floatType heightZoom = screenHeight / worldHeight;
    const floatType zoom = fmin(widthZoom, heightZoom);

    // Calculate the visible width and height
    const floatType visibleWidth = rightBorder - leftBorder;
    const floatType visibleHeight =
        visibleWidth * (static_cast<floatType>(GetScreenHeight()) / static_cast<floatType>(GetScreenWidth()));

    // Calculate the center of the camera view based on the borders
    const floatType centerX = (leftBorder + rightBorder) / 2.0f;
    const floatType centerY = (lowerBorder + upperBorder) / 2.0f;

    spdlog::info("lower border: {}", lowerBorder);
    spdlog::info("upper border: {}", upperBorder);

    const Vector2 screenCenter = {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2};

    // Initialize the camera
    Camera2D camera;
    camera.target = {centerX, centerY};
    camera.offset = screenCenter;
    camera.rotation = 0.0f;
    camera.zoom = zoom;

    spdlog::info("Screen center: ({},{})", screenCenter.x, screenCenter.y);
    spdlog::info("World center: ({},{})", centerX, centerY);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        Vector center = {camera.target.x, camera.target.y};
        Vector2 centerRendering = transformCoordinatesTesting(lowerBorder, upperBorder, center);
        DrawCircle(centerRendering.x, centerRendering.y, 100.0f, PINK);

        HikerConstants hikerConstants = ConfigManager::getInstance().getGameConstants().hikerConstants;

        BeginMode2D(camera);

        Vector basePoint = terrain.getPolyRepresentationOfGroundRendering()->getStartPoint();
        Vector2 basePointRendering = transformCoordinatesTesting(lowerBorder, upperBorder, basePoint);
        DrawRectangle(basePointRendering.x, basePointRendering.y - hikerConstants.hikerHeight,
                      hikerConstants.hikerWidth, hikerConstants.hikerHeight, GREEN);

        StaticPolyline const *ground = terrain.getPolyRepresentationOfGroundRendering();
        for (Vector point : ground->getPoints()) {
            Vector2 pointRendering = transformCoordinatesTesting(lowerBorder, upperBorder, point);
            DrawCircle(static_cast<int>(pointRendering.x), static_cast<int>(pointRendering.y), 4.0f, RED);
        }
        StaticPolyline const *basePoints = terrain.getBasepoints();
        for (Vector point : basePoints->getPoints()) {
            Vector2 pointRendering = transformCoordinatesTesting(lowerBorder, upperBorder, point);
            DrawCircle(static_cast<int>(pointRendering.x), static_cast<int>(pointRendering.y), 7.0f, GREEN);
        }

        EndMode2D();
        EndDrawing();
    }
}
// NOLINTEND

void testIntersection() {
    Vector start1 = {1, 1};
    Vector end1 = {4, 4};
    Vector start2 = {2, 4};
    Vector end2 = {3.5, -0.5};
    Vector intersection = {2.5, 2.5};

    std::optional<Vector> computedIntersection = Vector::getIntersection(start1, end1, start2, end2);

    // spdlog::debug("({}, {})", computedIntersection.x, computedIntersection.y);

    assert(computedIntersection.value() == intersection);

    start1 = {0, -1};
    end1 = {0, 1};
    start2 = {1, 0};
    end2 = {-1, 0};
    intersection = {0, 0};

    computedIntersection = Vector::getIntersection(start1, end1, start2, end2);

    assert(computedIntersection.value() == intersection);
}

Vector2 transformCoordinatesTesting(floatType minY, floatType maxY, Vector position) {
    return {position.x, minY + maxY - position.y};
}