//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_MOUSE_H
#define SURVIVING_SARNTAL_MOUSE_H

#include "InputDevice.h"
#include <functional>
/**
 * Carful: Mouse is NOT IMPLEMENTED!
 */
class Mouse : public InputDevice {
  public:
    ~Mouse() override;

  protected:
    std::list<GameEvent> getGameEvents() override;

  private:
    static const std::map<TriggerType, std::function<bool(int)>>
        RAYLIB_MAPPINGS;
};

#endif // SURVIVING_SARNTAL_MOUSE_H
