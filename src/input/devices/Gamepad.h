//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_GAMEPAD_H
#define SURVIVING_SARNTAL_GAMEPAD_H

#include "InputDevice.h"
#include <functional>
#include <queue>
/**
 * Represents a gamepad. Gamepads have an id, because raylib permits multiple
 * gamepads to be used at the same time.
 */
class Gamepad : public InputDevice {
  public:
    explicit Gamepad(int gamepadID);
    ~Gamepad() override;

  protected:
    const int gamepadID;
    std::queue<GameEvent> getGameEvents() override;

  private:
    static const std::map<TriggerType, std::function<bool(int, int)>> raylibMappings;
};

#endif // SURVIVING_SARNTAL_GAMEPAD_H
