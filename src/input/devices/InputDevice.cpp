//
// Created by felix-roehr on 5/4/24.
//

#include "InputDevice.h"
#include "raylib.h"
#include <iostream>

InputDevice::~InputDevice() = default;

GameEvent InputDevice::getGameEvent(DeviceEvent deviceEvent) const {
    if (INPUT_MAPPINGS.count(deviceEvent) > 0) {
        return INPUT_MAPPINGS.at(deviceEvent);
    }
    return {NO_EVENT, NO_AXIS, NO_MODIFICATION};
}
