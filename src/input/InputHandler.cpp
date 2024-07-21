//
// Created by felix-roehr on 5/4/24.
//

#include "InputHandler.h"
#include "devices/Gamepad.h"
#include "devices/Keyboard.h"
#include "devices/Mouse.h"
#include "raylib.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <mutex>

InputHandler::InputHandler() : inputConstants(ConfigManager::getInstance().getGameConstants().inputConstants) {
    this->devices.push_back(new Keyboard());
    spdlog::info("Keyboard added.");
}

InputHandler::~InputHandler() = default;

std::queue<GameEvent> InputHandler::getEvents() const {
    std::queue<GameEvent> events = {};
    for (auto device : this->devices) {
        auto eventsFromDevice = device->getGameEvents();
        while (!eventsFromDevice.empty()) {
            events.push(eventsFromDevice.front());
            eventsFromDevice.pop();
        }
    }
    return events;
}

std::vector<InputDevice *> InputHandler::getDevices() const { return this->devices; }

void InputHandler::initializeGamepads() {
    BeginDrawing();
    for (int i = 0; i < this->inputConstants.maxGamepads; i++) {
        if (IsGamepadAvailable(i)) {
            this->devices.push_back(new Gamepad(i));
            spdlog::info("Gamepad {} added.", i);
        }
    }
    EndDrawing();
}
bool InputHandler::gamepadsInitialized() const {
    if (std::any_of(this->devices.cbegin(), this->devices.cend(),
                    [](InputDevice const *device) { return device->getDevice() == DEVICE_GAMEPAD; })) {
        return true;
    }
    return false;
}