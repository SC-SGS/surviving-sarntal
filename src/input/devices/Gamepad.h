//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_GAMEPAD_H
#define SURVIVING_SARNTAL_GAMEPAD_H

#include "InputDevice.h"
#include <functional>
#include <optional>
#include <queue>
/**
 * Represents a gamepad. Gamepads have an id, because raylib permits multiple
 * gamepads to be used at the same time.
 */
class Gamepad : public InputDevice {
  public:
    explicit Gamepad(int gamepadID, const InputConstants &inputConstants);
    ~Gamepad() override;

  protected:
    std::queue<GameEvent> getGameEvents() override;

  private:
    double lastItemSwitchTime = 0.0;
    bool movingX = true;
    bool movingY = true;

    static const std::map<TriggerType, std::function<bool(int, int)>> raylibMappings;

    std::optional<GameEvent> convertDeviceEvent(const DeviceEvent &deviceEvent);

    std::optional<GameEvent> updateXMovementState(const DeviceEvent &deviceEvent);

    std::optional<GameEvent> updateYMovementState(const DeviceEvent &deviceEvent);
};

#endif // SURVIVING_SARNTAL_GAMEPAD_H
