//
// Created by felix-roehr on 5/4/24.
//

#include "InputDevice.h"
#include "raylib.h"
#include <iostream>

InputDevice::~InputDevice() = default;

GameEvent InputDevice::getGameEvent(DeviceEvent deviceEvent) const {
    // std::cout << INPUT_MAPPINGS.at(deviceEvent).type << std::endl;
    if (INPUT_MAPPINGS.count(deviceEvent) > 0) {
        // std::cout << "BOO" << std::endl;
        // std::cout << "Game event type: " << INPUT_MAPPINGS.at(deviceEvent).type << std::endl;
        return INPUT_MAPPINGS.at(deviceEvent);
    }
    return {NO_EVENT, NO_AXIS, NO_MODIFICATION};
}
