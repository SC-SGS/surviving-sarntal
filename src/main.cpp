#include "game/Game.hpp"
#include "game/GameFactory.hpp"
#include "menu/MenuEventProcessor.h"
#include "spawner/PolygonGenerator.h"
#include "spdlog/fmt/bundled/chrono.h"
#include "spdlog/spdlog.h"

void testIntersection();
void renderTerrainTest(Terrain terrain);
Vector2 transformCoordinatesTesting(floatType minY, floatType maxY, Vector position);

// TODO extract initialization to game
int main(int argc, char *argv[]) { // NOLINT [readability-function-size,-warnings-as-errors]
    spdlog::set_level(spdlog::level::info);
    testIntersection();
    InitWindow(graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL, "Surviving Sarntal");

    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);
    SDL_Init(SDL_INIT_GAMECONTROLLER);

    Camera2D camera{0};
    GameFactory gameFactory{camera};
    Game game = gameFactory.buildGame();
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