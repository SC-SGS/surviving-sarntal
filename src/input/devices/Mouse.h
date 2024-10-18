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
    explicit Mouse(const InputConstants &inputConstants);
    ~Mouse() override;

  protected:
    std::queue<GameEvent> getGameEvents() override;

  private:
    static const std::map<TriggerType, std::function<bool(int)>> raylibMappings;
};

#endif // SURVIVING_SARNTAL_MOUSE_H
