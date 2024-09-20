//
// Created by bjoern on 5/31/24.
//

#ifndef GAME_HPP
#define GAME_HPP
#include "../output/audio/AudioService.hpp"
#include "../output/graphics/Renderer.h"
#include "../physics/PhysicsEngine.hpp"
#include "../utilities/Singleton.hpp"

#include <memory>

class Game {

  public:
    Game(World &world, Renderer &renderer, PhysicsEngine &physicsEngine, AudioService &audioService,
         InputHandler &inputHandler, GameConstants &gameConstants);
    ~Game() = default;
    void run();

    // TODO renderer and different renderer for Round?
    // TODO initialize physics engine and world per round
    // TODO start and menu etc. must be handled differently ig, because there is no world but something is rendered and
    // TODO there is no pe but input is handled
    void playRound() const;

    bool debugMode = false;

  private:
    World &world;
    Renderer &renderer;
    PhysicsEngine &physicsEngine;
    AudioService &audioService;
    InputHandler &inputHandler;
    GameConstants &gameConstants;

    static void drawEndScreen();
};

#endif // GAME_HPP
