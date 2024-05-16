//
// Created by felix-roehr on 5/4/24.
//

#include "InputDevice.h"

InputDevice::~InputDevice() = default;

GameEvent InputDevice::getGameEvent(DeviceEvent deviceEvent) const {
    auto iter = INPUT_MAPPINGS.find(deviceEvent);
    if (iter != INPUT_MAPPINGS.end()) {
        return iter->second;
    }
    return {NO_EVENT, NO_AXIS, NO_MODIFICATION};
}
