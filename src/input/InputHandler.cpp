//
// Created by felix-roehr on 5/4/24.
//

#include "InputHandler.h"
#include "devices/Gamepad.h"
#include "devices/Keyboard.h"
#include "devices/Mouse.h"
#include "raylib.h"
#include "spdlog/spdlog.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <mutex>

InputHandler::InputHandler() : inputConstants(ConfigManager::getInstance().getGameConstants().inputConstants) {
    this->devices.push_back(new Keyboard(this->inputConstants));
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

void InputHandler::initializeGamepads(int remainingSeconds) {
    BeginDrawing();
    ClearBackground(BLACK);
    std::string text = "To initialize gamepad, press any button on the gamepad";
    DrawText(text.c_str(), GetScreenWidth() / 2, GetScreenHeight() / 2, 30, WHITE);
    DrawText(std::to_string(remainingSeconds).c_str(), GetScreenWidth() / 2, GetScreenHeight() / 2 + 50, 30, GREEN);
    for (int i = 0; i < this->inputConstants.maxGamepads; i++) {
        if (IsGamepadAvailable(i)) {
            this->devices.push_back(new Gamepad(i, this->inputConstants));
            spdlog::info("Gamepad {} added.", i);
            SDL_Init(SDL_INIT_GAMECONTROLLER);
        }
    }
    EndDrawing();
}
bool InputHandler::gamepadsInitialized() const {
    return std::any_of(this->devices.cbegin(), this->devices.cend(),
                       [](InputDevice const *device) { return device->getDevice() == DEVICE_GAMEPAD; });
}