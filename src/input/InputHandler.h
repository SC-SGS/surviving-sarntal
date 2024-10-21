//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_INPUTHANDLER_H
#define SURVIVING_SARNTAL_INPUTHANDLER_H

#include "../game/GameProperties.hpp"
#include "../output/ResourceManager.h"
#include "../utilities/ConfigManager.hpp"
#include "../utilities/Singleton.hpp"
#include "devices/Device.h"
#include "devices/InputDevice.h"
#include "events/GameEvent.h"
#include "raylib.h"
#include <list>
#include <memory>

/**
 * An input handler is used to process the inputs from a given device map them
 * to the corresponding game events.
 */
class InputHandler {
  public:
    /**
     * Creates the input handler with the device currently available. If both
     * keyboard and gamepad are available, the gamepad will be used.
     */
    explicit InputHandler(ResourceManager &resourceManager);
    ~InputHandler();

    /**
     * Checks for events currently happening to control the game.
     *
     * @return list of events
     */
    std::queue<GameEvent> getEvents() const;

    /**
     * Update the input Handler to also recognize gamepads as input
     */
    void initializeGamepads(int remainingSeconds);

    /**
     * Checks whether gamepads have been initialized.
     *
     * @return
     */
    bool gamepadsInitialized() const;

    std::vector<InputDevice *> getDevices() const;

  private:
    std::vector<InputDevice *> devices;
    InputConstants inputConstants;
    ResourceManager &resourceManager;
};

#endif // SURVIVING_SARNTAL_INPUTHANDLER_H
