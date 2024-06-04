#include "game/Game.hpp"
int main(int argc, char *argv[]) {
    InitWindow(graphics::SCREEN_WIDTH, graphics::SCREEN_HEIGHT, "Surviving Sarntal");
    SetTargetFPS(60);
    Game::getInstance().run();
}