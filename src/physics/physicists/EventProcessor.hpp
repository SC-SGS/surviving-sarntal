//
// Created by bjoern on 5/21/24.
//

#ifndef EVENTPROCESSOR_H
#define EVENTPROCESSOR_H

#include "../../entities/World.h"
#include "../../input/InputHandler.h"
#include "../../input/events/GameEvent.h"

#include <list>

class EventProcessor {
    using EventFunction = void (EventProcessor::*)(GameEvent) const;

  public:
    explicit EventProcessor(World &world, const InputHandler &inputHandler);

    /**
     * Performs the changes to the world caused by the user input.
     */
    void processEvents();

  private:
    World &world;
    const InputHandler &inputHandler;

    std::map<GameEvent, EventFunction> functionMappings;

    // TODO pls doc
    void crouch(GameEvent event) const;
    void uncrouch(GameEvent event) const;
    void pickItem(GameEvent event) const;
    void dropItem(GameEvent event) const;
    void useItem(GameEvent event) const;
    void jump(GameEvent event) const;
    void pause(GameEvent event) const;
    void specialAbility(GameEvent event) const;
    void toggleDebug(GameEvent event) const;
    void fullscreen(GameEvent event) const;

    void switchItem(GameEvent event) const;
    void moveX(GameEvent event) const;
    void moveY(GameEvent event) const;

    void noEvent(GameEvent event) const;
};

#endif // EVENTPROCESSOR_H
