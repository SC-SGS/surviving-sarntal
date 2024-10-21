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

InputHandler::InputHandler(ResourceManager &resourceManager)
    : inputConstants(ConfigManager::getInstance().getGameConstants().inputConstants), resourceManager(resourceManager) {
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
    Texture2D backgroundTexture = this->resourceManager.getTexture("recognizeGamepadBackground");
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(backgroundTexture.width),
                                 static_cast<float>(backgroundTexture.height)};
    const Rectangle destRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    DrawTexturePro(backgroundTexture, sourceRec, destRec, {0, 0}, 0, WHITE);
    int fontSize = this->inputConstants.gamepadInitializingCountdownFontSize;
    int textWidth = MeasureText(std::to_string(remainingSeconds).c_str(), fontSize);
    DrawText(std::to_string(remainingSeconds).c_str(), (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 + 50,
             fontSize, ORANGE);
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