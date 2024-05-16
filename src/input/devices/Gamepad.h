//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_GAMEPAD_H
#define SURVIVING_SARNTAL_GAMEPAD_H

#include "InputDevice.h"
#include <functional>
/**
 * Represents a gamepad. Gamepads have an id, because raylib permits multiple
 * gamepads to be used at the same time.
 */
class Gamepad : public InputDevice {
  public:
    explicit Gamepad(int id);
    ~Gamepad() override;

  protected:
    const int id;
    std::list<GameEvent> getGameEvents() override;

  private:
    static const std::map<TriggerType, std::function<bool(int, int)>>
        RAYLIB_MAPPINGS;
};

#endif // SURVIVING_SARNTAL_GAMEPAD_H
