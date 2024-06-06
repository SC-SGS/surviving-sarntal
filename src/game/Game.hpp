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

class Game : public Singleton<Game> {
    friend class Singleton<Game>;

  public:
    void run() const;

    // TODO renderer and different renderer for Round?
    // TODO initialize physics engine and world per round
    // TODO start and menu etc. must be handled differently ig, because there is no world but something is rendered and
    // TODO there is no pe but input is handled
    void playRound() const;

  private:
    World &world = World::getInstance();
    Renderer &renderer = Renderer::getInstance();
    PhysicsEngine &physicsEngine = PhysicsEngine::getInstance();
    AudioService &audioService = AudioService::getInstance();
    InputHandler &inputHandler = InputHandler::getInstance();

    static void drawEndScreen();
    Game();
    ~Game();
};

#endif // GAME_HPP
