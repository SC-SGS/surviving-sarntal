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
    if (this->gameConstants.audioConstants.musicEnabled) {
        audioService.playSong("background-music", true, this->gameConstants.audioConstants.musicVolume);
    }

    int startTime = (int)floorf64(GetTime());
    int offset = this->gameConstants.inputConstants.gamepadInitializingTime;
    while (this->shouldRunGame()) {
        audioService.updateMusicStream();
        int currentTime = (int)floorf64(GetTime());
        bool needToInitGamePads = currentTime < startTime + offset && !this->inputHandler.gamepadsInitialized();
        if (needToInitGamePads) {
            initializeGamepads(startTime + offset - currentTime);
        } else {
            this->gameTick();
        }
    }
}

bool Game::shouldRunGame() { return !WindowShouldClose() && !this->menuEngine.isGameClosed(); }

void Game::gameTick() {
    audioService.updateMusicStream();

    if (!menuEngine.isGamePlayRunning()) {
        this->physicsEngine.pause();
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
        this->physicsEngine.pause();
    }
}

void Game::initializeGamepads(int remainingSeconds) {
    this->inputHandler.initializeGamepads(remainingSeconds);
    if (this->inputHandler.gamepadsInitialized()) {
        spdlog::info("Gamepads initialized at time {}", GetTime());
    }
}

void Game::runMenu() {
    std::queue<GameEvent> events = this->inputHandler.getEvents();
    this->menuEventProcessor.addEvents(events);
    this->menuEventProcessor.processEvents();
    this->menuEventProcessor.clearRepeatedEvents();
    if (!this->menuEngine.isGameClosed()) {
        this->menuRenderer.render();
    }
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
    menuEngine.setAchievedGameScore(this->world.getGameScore());
}

void Game::resetGame() {
    playedEndSound = false;
    this->physicsEngine.reset();
    this->world.reset();
    this->renderer.reset();
    this->physicsEngine.reset();
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