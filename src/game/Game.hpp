//
// Created by bjoern on 5/31/24.
//

#ifndef GAME_HPP
#define GAME_HPP
#include "../menu/FullMenuRenderer.h"
#include "../menu/MenuEventProcessor.h"
#include "../menu/MenuRenderer.h"
#include "../output/audio/AudioService.hpp"
#include "../output/graphics/Renderer.h"
#include "../physics/PhysicsEngine.hpp"
#include "../utilities/Singleton.hpp"

#include <memory>

class Game {

  public:
    Game(World &world,
         Renderer &renderer,
         FullMenuRenderer &menuRenderer,
         MenuEngine &menuEngine,
         MenuEventProcessor &menuEventProcessor,
         PhysicsEngine &physicsEngine,
         AudioService &audioService,
         InputHandler &inputHandler,
         GameConstants &gameConstants);
    ~Game() = default;
    void run();

    // TODO renderer and different renderer for Round?
    // TODO initialize physics engine and world per round
    // TODO start and menu etc. must be handled differently ig, because there is no world but something is rendered and
    // TODO there is no pe but input is handled
    void playRound() const;

    bool debugMode = false;
    bool gameplayStarted = false;
    void pause();

  private:
    World &world;
    Renderer &renderer;
    FullMenuRenderer &menuRenderer;
    MenuEngine &menuEngine;
    MenuEventProcessor &menuEventProcessor;
    PhysicsEngine &physicsEngine;
    AudioService &audioService;
    InputHandler &inputHandler;
    GameConstants &gameConstants;

    void gameLoop();
    void initializeGamepads();
    void runMenu();
    void runGameplay();
    void endGameplay();
    void resetGame();
    void checkPlayAgainClicked();

    bool playedEndSound = false;
    bool gamePaused = false;
    void checkPause();
};

#endif // GAME_HPP
