//
// Created by bjoern on 5/31/24.
//

#include "Game.hpp"

#include <mutex>

Game::Game() { std::cout << "Game initialized." << std::endl; }

Game::~Game() { std::cout << "Game destroyed." << std::endl; }

void Game::run() const {
    // game loop
    while (this->world.getHiker().getIsAlive() && !WindowShouldClose()) {
        std::queue<GameEvent> events = this->inputHandler.getEvents();
        this->physicsEngine.update(events);
        this->renderer.draw();
    }
}