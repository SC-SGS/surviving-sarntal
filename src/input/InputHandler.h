//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_INPUTHANDLER_H
#define SURVIVING_SARNTAL_INPUTHANDLER_H

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
class InputHandler : public Singleton<InputHandler> {
    friend class Singleton<InputHandler>; // Allow Singleton to access the constructor??

  private:
    static const int maxGamepads = 8;
    InputDevice *device;

    /**
     * Creates the input handler with the device currently available. If both
     * keyboard and gamepad are available, the gamepad will be used.
     */
    InputHandler();
    ~InputHandler();

  public:
    /**
     * Creates an input handler with the specified device.
     */
    explicit InputHandler(Device device);

    /**
     * Checks for events currently happening to control the game.
     *
     * @return list of events
     */
    std::queue<GameEvent> getEvents() const;
};

#endif // SURVIVING_SARNTAL_INPUTHANDLER_H
