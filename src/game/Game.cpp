//
// Created by bjoern on 5/31/24.
//

#include "Game.hpp"
#include "GameProperties.hpp"
#include "spdlog/spdlog.h"
#include <mutex>

Game::Game(World &world,
           Renderer &renderer,
           FullMenuRenderer &menuRenderer,
           MenuEngine &menuEngine,
           MenuEventProcessor &menuEventProcessor,
           PhysicsEngine &physicsEngine,
           AudioService &audioService,
           InputHandler &inputHandler,
           GameConstants &gameConstants)
    : world(world),
      renderer(renderer),
      menuRenderer(menuRenderer),
      menuEngine(menuEngine),
      menuEventProcessor(menuEventProcessor),
      physicsEngine(physicsEngine),
      audioService(audioService),
      inputHandler(inputHandler),
      gameConstants(gameConstants) {
    spdlog::info("Game initialized.");
}

void Game::run() {
    audioService.playSound("background-music");
    // bool playedEndSound = false;

    while (!WindowShouldClose()) {
        bool needToInitGamePads = GetTime() < 5.0f && !this->inputHandler.gamepadsInitialized();
        if (needToInitGamePads) {
            initializeGamepads();
        }
        this->gameLoop();
    }
}

void Game::gameLoop() {
    if (!menuEngine.isGamePlayRunning()) {
        this->checkPlayAgainClicked();
        runMenu();
        return;
    }
    bool gamePlayRunning = this->world.getHiker().getIsAlive() && !gamePaused;
    if (gamePlayRunning) {
        this->checkPause();
        runGameplay();
    } else {
        endGameplay();
    }
}

void Game::checkPause() {
    if (!menuEngine.isGamePlayRunning()) {
        gamePaused = true;
    }
}

void Game::initializeGamepads() {
    this->inputHandler.initializeGamepads();
    if (this->inputHandler.gamepadsInitialized()) {
        spdlog::info("Gamepads initialized at time {}", GetTime());
    }
}

void Game::runMenu() {
    std::queue<GameEvent> events = this->inputHandler.getEvents();
    this->menuEventProcessor.addEvents(events);
    this->menuEventProcessor.processEvents();
    this->menuEventProcessor.clearRepeatedEvents();
    this->menuRenderer.render();
}

void Game::runGameplay() {
    // this->inputHandler.update();
    std::queue<GameEvent> events = this->inputHandler.getEvents();
    this->physicsEngine.update(events);
    this->renderer.draw();
    this->world.updateGameScore();
}

void Game::endGameplay() {
    if (!playedEndSound) {
        audioService.playSound("game-over");
        playedEndSound = true;
    }
    menuEngine.setGameplayRunning(false);
    menuEngine.switchTo(END_SCREEN);
}

void Game::resetGame() {
    playedEndSound = false;
    this->world.reset();
    this->renderer.reset();
    this->renderer.getMountainRenderer().reset();
}

void Game::checkPlayAgainClicked() {
    if (menuEngine.getPlayAgainClicked()) {
        this->resetGame();
        menuEngine.setPlayAgainClicked(false);
    }
}

void Game::pause() {
    this->gamePaused = true;
    // this->world.pause();
}