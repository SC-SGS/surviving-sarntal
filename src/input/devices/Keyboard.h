//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_KEYBOARD_H
#define SURVIVING_SARNTAL_KEYBOARD_H

#include "InputDevice.h"
#include <functional>
/**
 * Represents a keyboard.
 */
class Keyboard : public InputDevice {
  public:
    Keyboard();
    ~Keyboard() override;

  protected:
    std::list<GameEvent> getGameEvents() override;

  private:
    static const std::map<TriggerType, std::function<bool(int)>>
        RAYLIB_MAPPINGS;
};

#endif // SURVIVING_SARNTAL_KEYBOARD_H
