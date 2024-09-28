#include "game/DevMode.hpp"
#include "game/Game.hpp"
#include "game/GameFactory.hpp"
#include "output/haptics/HapticsService.hpp"
#include "spdlog/fmt/bundled/chrono.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::info);
    InitWindow(graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL, "Surviving Sarntal");

    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);
    SDL_Init(SDL_INIT_GAMECONTROLLER);

    Camera2D camera{};
    const GameFactory gameFactory{camera};
    Game game = gameFactory.buildGame();
    game.run();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}