#include "game/DevMode.hpp"
#include "game/Game.hpp"
#include "game/GameFactory.hpp"
#include "output/haptics/HapticsService.hpp"
#include "spdlog/fmt/bundled/chrono.h"
#include "spdlog/spdlog.h"

void initRaylib() {
    InitWindow(0, 0, "Surviving Sarntal");

    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);
}

void handleFullScreen() {
    ConfigManager &configManager = ConfigManager::getInstance();

    if (configManager.getFullscreen()) {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        BeginDrawing();
        EndDrawing();
        ToggleFullscreen();
    } else {
        SetWindowSize(graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL);
    }
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::info);

    initRaylib();
    handleFullScreen();

    Camera2D camera{};
    GameFactory gameFactory{camera};

    ConfigManager &configManager = ConfigManager::getInstance();
    if (!configManager.isInDevMode()) {
        Game game = gameFactory.buildGame();
        game.run();
    } else {
        DevMode devMode = gameFactory.buildDevMode();
        devMode.run();
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}