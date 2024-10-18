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

    void gameTick();
    void initializeGamepads(int remainingSeconds);
    void runMenu();
    void runGameplay();
    void endGameplay();
    void resetGame();
    void checkPlayAgainClicked();

    bool playedEndSound = false;
    bool gamePaused = false;
    void checkPause();
    bool shouldRunGame();
};

#endif // GAME_HPP
