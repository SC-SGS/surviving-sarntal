//
// Created by bjoern on 5/21/24.
//

#ifndef EVENTPROCESSOR_H
#define EVENTPROCESSOR_H

#include "../../entities/World.h"
#include "../../input/InputHandler.h"
#include "../../input/events/GameEvent.h"
#include "../../menu/EventProcessor.h"
#include "../../menu/MenuEngine.h"
#include "../../output/graphics/Renderer.h"
#include "../../utilities/Singleton.hpp"

class GameEventProcessor final : public EventProcessor {

  public:
    explicit GameEventProcessor(World &world,
                                Renderer &renderer,
                                const GameConstants &gameConstants,
                                MenuEngine &menuEngine,
                                AudioService &audioService);

  private:
    // Dependencies
    World &world;
    Renderer &renderer;
    MenuEngine &menuEngine;
    const GameConstants &gameConstants;
    AudioService &audioService;

    /**
     * Puts the Hiker into the crouched state.
     *
     * @param event crouch event
     */
    void crouch(GameEvent event) const;

    /**
     * Puts the Hiker into the uncrouched state.
     *
     * @param event uncrouch event
     */
    void uncrouch(GameEvent event) const;

    /**
     * Picks all Items in range that can't be collected automatically
     *
     * @param event
     */
    void pickItem(GameEvent event) const;

    /**
     * Picks all items in range that can be collected automatically.
     */
    void pickAutoCollectableItems() const;

    /**
     * This method drops the currently selected item from the inventory.
     * @param event
     */
    void dropItem(GameEvent event) const;

    /**
     * This method uses the currently selected item.
     * @param event
     */
    void useItem(GameEvent event) const;

    /**
     * This method puts the hiker in a jumping state.
     * @param event
     */
    void jump(GameEvent event) const;

    /**
     * This method pauses the game.
     * @param event
     */
    void pause(GameEvent event);

    void specialAbility(GameEvent event) const;

    void toggleDebug(GameEvent event) const;

    void fullscreen(GameEvent event) const;

    /**
     * This method switches the currently selected item in the inventory.
     *
     * @param event
     */
    void switchItem(GameEvent event) const;

    /**
     * This method is responsible for adapting the hiker's speed based on user input.
     * If the user clicks the button that moves the hiker to the right, the speed is increased.
     * If the user clicks the button that moves the hiker to the left, the speed is decreased.
     * Also plays the movement sound
     *
     * @param event
     */
    void moveX(GameEvent event);
    void moveY(GameEvent event) const;

    void noEvent(GameEvent event) const;

    void pickItem(const std::shared_ptr<Item> &item) const;

    floatType getCoinSpeedFactor() const;

    floatType calculateHikerSpeed(floatType factor) const;
    floatType getHikerMovementFactor() const;
};

#endif // EVENTPROCESSOR_H
