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

InputHandler::InputHandler() {
    bool deviceSet = false;
    for (int i = 0; i < maxGamepads; i++) {
        if (IsGamepadAvailable(i) && !deviceSet) {
            this->device = new Gamepad(i);
            deviceSet = true;
        }
    }
    if (!deviceSet) {
        this->device = new Keyboard();
    }
    spdlog::info("InputHandler constructed");
}

InputHandler::~InputHandler() = default;

InputHandler::InputHandler(const Device device) {
    switch (device) {
    case DEVICE_GAMEPAD:
        for (int i = 0; i < maxGamepads; i++) {
            if (IsGamepadAvailable(i)) {
                this->device = new Gamepad(i);
            }
        }
        break;
    case DEVICE_KEYBOARD:
        this->device = new Keyboard();
        break;
    case DEVICE_MOUSE:
        this->device = new Mouse();
        break;
    default:
        this->device = nullptr;
        break;
    }
}

std::queue<GameEvent> InputHandler::getEvents() const { return device->getGameEvents(); }