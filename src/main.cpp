#include "game/Game.hpp"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::info);
    InitWindow(graphics::SCREEN_WIDTH, graphics::SCREEN_HEIGHT, "Surviving Sarntal");
    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);
    SDL_Init(SDL_INIT_GAMECONTROLLER);
    Game::getInstance().run();
    return 0;
}