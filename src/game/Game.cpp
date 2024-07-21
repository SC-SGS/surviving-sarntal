//
// Created by bjoern on 5/31/24.
//

#include "Game.hpp"
#include "GameProperties.hpp"
#include "spdlog/spdlog.h"
#include <mutex>

Game::Game(World &world, Renderer &renderer, PhysicsEngine &physicsEngine, AudioService &audioService,
           InputHandler &inputHandler, GameConstants gameConstants)
    : world(world), renderer(renderer), physicsEngine(physicsEngine), audioService(audioService),
      inputHandler(inputHandler), gameConstants(gameConstants) {
    spdlog::info("Game initialized.");
}

void Game::run() {
    // game loop
    audioService.playSound("background-music");
    bool playedEndSound = false;

    while (!WindowShouldClose()) {
        if (GetTime() < 5.0f && !this->inputHandler.gamepadsInitialized()) {
            this->inputHandler.initializeGamepads();
            if (this->inputHandler.gamepadsInitialized()) {
                spdlog::info("Gamepads initialized at time {}", GetTime());
            }
        } else if (this->world.getHiker().getIsAlive()) {
            // this->inputHandler.update();
            std::queue<GameEvent> events = this->inputHandler.getEvents();
            this->physicsEngine.update(events);
            this->renderer.draw();
            this->world.updateGameScore();
        } else {
            if (!playedEndSound) {
                audioService.playSound("game-over");
                playedEndSound = true;
            }
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