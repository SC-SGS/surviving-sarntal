//
// Created by felix-roehr on 5/4/24.
//

#include "Gamepad.h"
#include "../../game/GameProperties.hpp"
#include "raylib.h"
#include "spdlog/spdlog.h"
#include <optional>

Gamepad::Gamepad(int gamepadID, const InputConstants &inputConstants)
    : InputDevice(gamepadID, DEVICE_GAMEPAD, inputConstants) {
    INPUT_MAPPINGS = {
        // Normal events
        // game
        {{GAMEPAD_BUTTON_RIGHT_FACE_DOWN, TRIGGER_PRESSED}, {JUMP, NO_AXIS, NO_MODIFICATION, false}},
        {{GAMEPAD_BUTTON_RIGHT_TRIGGER_1, TRIGGER_DOWN}, {CROUCH, NO_AXIS, NO_MODIFICATION, false}},
        {{GAMEPAD_BUTTON_RIGHT_TRIGGER_1, TRIGGER_RELEASED}, {UNCROUCH, NO_AXIS, NO_MODIFICATION, false}},
        {{GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, TRIGGER_PRESSED}, {ITEM_PICK, NO_AXIS, NO_MODIFICATION, false}},
        {{GAMEPAD_BUTTON_RIGHT_FACE_LEFT, TRIGGER_PRESSED}, {ITEM_USE, NO_AXIS, NO_MODIFICATION, false}},
        {{GAMEPAD_BUTTON_RIGHT_FACE_UP, TRIGGER_PRESSED}, {ITEM_DROP, NO_AXIS, NO_MODIFICATION, false}},
        //{{GAMEPAD_BUTTON_RIGHT_TRIGGER_2, TRIGGER_PRESSED}, {SPECIAL_ABILITY, NO_AXIS, NO_MODIFICATION, false}},
        {{GAMEPAD_BUTTON_MIDDLE_RIGHT, TRIGGER_PRESSED}, {PAUSE, NO_AXIS, NO_MODIFICATION, false}},
        // menu
        {{GAMEPAD_BUTTON_LEFT_TRIGGER_1, TRIGGER_PRESSED}, {SELECT_BUTTON, NO_AXIS, 0, false}},

        // Virtual axis modifications
        // menu
        {{GAMEPAD_BUTTON_RIGHT_TRIGGER_2, TRIGGER_PRESSED}, {AXIS_MODIFICATION, BUTTON_SWITCH, 1, false}},
        {{GAMEPAD_BUTTON_LEFT_TRIGGER_2, TRIGGER_PRESSED}, {AXIS_MODIFICATION, BUTTON_SWITCH, -1, false}},
        // game
        {{GAMEPAD_BUTTON_LEFT_FACE_RIGHT, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, 1, true}},
        {{GAMEPAD_BUTTON_LEFT_FACE_LEFT, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, -1, true}},

        // Axis modifications
        {{GAMEPAD_AXIS_LEFT_X, TRIGGER_POSITION}, {AXIS_MODIFICATION, MOVEMENT_X, 0, true}},
        {{GAMEPAD_AXIS_LEFT_Y, TRIGGER_POSITION}, {AXIS_MODIFICATION, MOVEMENT_Y, 0, true}},
        {{GAMEPAD_AXIS_RIGHT_X, TRIGGER_POSITION}, {AXIS_MODIFICATION, ITEM_SWITCH, 0, false}},
    };
}

const std::map<TriggerType, std::function<bool(int, int)>> Gamepad::raylibMappings = {
    {TRIGGER_PRESSED, IsGamepadButtonPressed},
    {TRIGGER_DOWN, IsGamepadButtonDown},
    {TRIGGER_RELEASED, IsGamepadButtonReleased},
    {TRIGGER_UP, IsGamepadButtonUp},
};

Gamepad::~Gamepad() = default;

std::queue<GameEvent> Gamepad::getGameEvents() {
    std::queue<GameEvent> events = {};
    for (auto it = INPUT_MAPPINGS.cbegin(); it != INPUT_MAPPINGS.end(); ++it) {
        DeviceEvent deviceEvent = it->first;
        auto gameEvent = this->convertDeviceEvent(deviceEvent);
        if (gameEvent.has_value()) {
            events.push(gameEvent.value());
        }
    }
    return events;
}

std::optional<GameEvent> Gamepad::convertDeviceEvent(const DeviceEvent &deviceEvent) {
    if (deviceEvent.triggerType == TRIGGER_POSITION) {
        floatType axisValue = GetGamepadAxisMovement(this->getId(), deviceEvent.trigger);
        if (deviceEvent.trigger == GAMEPAD_AXIS_LEFT_X) {
            return this->updateXMovementState(deviceEvent);
        } else if (deviceEvent.trigger == GAMEPAD_AXIS_LEFT_Y) {
            return this->updateYMovementState(deviceEvent);
        } else if (deviceEvent.trigger == GAMEPAD_AXIS_RIGHT_X) {
            if (GetTime() - this->lastItemSwitchTime > this->inputConstants.gamepadItemSwitchCooldown &&
                fabsf(axisValue) > this->inputConstants.itemSwitchThreshold) {
                this->lastItemSwitchTime = GetTime();
                axisValue = axisValue / fabsf(axisValue);
            } else {
                return std::nullopt;
            }
        }
        GameEvent gameEvent = getGameEvent(deviceEvent);
        gameEvent.axisValue = axisValue;
        return gameEvent;
    } else if (Gamepad::raylibMappings.at(deviceEvent.triggerType)(this->getId(), deviceEvent.trigger)) {
        return getGameEvent(deviceEvent);
    }
    return std::nullopt;
}

std::optional<GameEvent> Gamepad::updateXMovementState(const DeviceEvent &deviceEvent) {
    assert(deviceEvent.trigger == GAMEPAD_AXIS_LEFT_X);
    floatType axisValue = GetGamepadAxisMovement(this->getId(), deviceEvent.trigger);
    if (fabsf(axisValue) < NUMERIC_EPSILON) {
        if (!this->movingX) {
            return std::nullopt;
        } else {
            this->movingX = false;
        }
    } else {
        this->movingX = true;
    }
    GameEvent gameEvent = getGameEvent(deviceEvent);
    gameEvent.axisValue = axisValue;
    return gameEvent;
}

std::optional<GameEvent> Gamepad::updateYMovementState(const DeviceEvent &deviceEvent) {
    assert(deviceEvent.trigger == GAMEPAD_AXIS_LEFT_Y);
    floatType axisValue = GetGamepadAxisMovement(this->getId(), deviceEvent.trigger);
    if (fabsf(axisValue) < NUMERIC_EPSILON) {
        if (!this->movingY) {
            return std::nullopt;
        } else {
            this->movingY = false;
        }
    } else {
        this->movingY = true;
    }
    GameEvent gameEvent = getGameEvent(deviceEvent);
    gameEvent.axisValue = axisValue;
    return gameEvent;
}
