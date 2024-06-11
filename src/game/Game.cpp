//
// Created by bjoern on 5/31/24.
//

#include "Game.hpp"
#include "../utilities/GameConstants.hpp"
#include "spdlog/spdlog.h"
#include <mutex>

Game::Game() { spdlog::info("Game initialized."); }

Game::~Game() { spdlog::info("Game destroyed."); }

void Game::run() const {
    // game loop
    while (!WindowShouldClose()) {
        if (this->world.getHiker().getIsAlive()) {
            std::queue<GameEvent> events = this->inputHandler.getEvents();
            this->physicsEngine.update(events);
            this->renderer.draw();
        } else {
            drawEndScreen();
        }
    }
}
void Game::drawEndScreen() {

    const char *message = "Busted!";
    int fontSize = 40;

    // Calculate the text width and height
    int textWidth = MeasureText(message, fontSize);
    int textHeight = fontSize; // Ascent + Descent, but raylib doesn't provide this separately, so using font size

    // Calculate the positions
    int posX = (graphics::SCREEN_WIDTH - textWidth) / 2;
    int posY = (graphics::SCREEN_HEIGHT - textHeight) / 2;

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawText(message, posX, posY, fontSize, RED);

    EndDrawing();
}
