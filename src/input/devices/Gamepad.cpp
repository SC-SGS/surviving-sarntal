//
// Created by felix-roehr on 5/4/24.
//

#include "Gamepad.h"
#include "raylib.h"

Gamepad::Gamepad(int id) : id(id) {
    INPUT_MAPPINGS = {
        // Normal events
        {{GAMEPAD_BUTTON_RIGHT_FACE_DOWN, TRIGGER_PRESSED}, {JUMP, NO_AXIS, NO_MODIFICATION}},
        {{GAMEPAD_BUTTON_RIGHT_TRIGGER_1, TRIGGER_DOWN}, {DUCK, NO_AXIS, NO_MODIFICATION}},
        {{GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, TRIGGER_PRESSED}, {ITEM_PICK, NO_AXIS, NO_MODIFICATION}},
        {{GAMEPAD_BUTTON_RIGHT_FACE_LEFT, TRIGGER_PRESSED}, {ITEM_USE, NO_AXIS, NO_MODIFICATION}},
        {{GAMEPAD_BUTTON_RIGHT_FACE_UP, TRIGGER_PRESSED}, {ITEM_DROP, NO_AXIS, NO_MODIFICATION}},
        {{GAMEPAD_BUTTON_RIGHT_TRIGGER_2, TRIGGER_PRESSED}, {SPECIAL_ABILITY, NO_AXIS, NO_MODIFICATION}},
        {{GAMEPAD_BUTTON_MIDDLE_RIGHT, TRIGGER_PRESSED}, {PAUSE, NO_AXIS, NO_MODIFICATION}},

        // Virtual axis modifications
        {{GAMEPAD_BUTTON_LEFT_FACE_RIGHT, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, INCREASE}},
        {{GAMEPAD_BUTTON_LEFT_FACE_LEFT, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, DECREASE}},

        // Axis modifications
        {{GAMEPAD_AXIS_LEFT_X, TRIGGER_POSITION}, {AXIS_MODIFICATION, MOVEMENT_X, 0}},
        {{GAMEPAD_AXIS_LEFT_Y, TRIGGER_POSITION}, {AXIS_MODIFICATION, MOVEMENT_Y, 0}},
        {{GAMEPAD_AXIS_RIGHT_X, TRIGGER_POSITION}, {AXIS_MODIFICATION, ITEM_SWITCH, 0}},
        //{{GAMEPAD_AXIS_RIGHT_X, TRIGGER_POSITION},{AXIS_MODIFICATION,
        // SECONDARY_X, 0}},
        {{GAMEPAD_AXIS_RIGHT_Y, TRIGGER_POSITION}, {AXIS_MODIFICATION, SECONDARY_Y, 0}},
    };
}

const std::map<TriggerType, std::function<bool(int, int)>> Gamepad::RAYLIB_MAPPINGS = {
    {TRIGGER_PRESSED, IsGamepadButtonPressed},
    {TRIGGER_DOWN, IsGamepadButtonDown},
    {TRIGGER_RELEASED, IsGamepadButtonReleased},
    {TRIGGER_UP, IsGamepadButtonUp},
    {TRIGGER_POSITION, GetGamepadAxisMovement}};

Gamepad::~Gamepad() = default;

std::list<GameEvent> Gamepad::getGameEvents() {
    std::list<GameEvent> events = {};
    for (auto it = INPUT_MAPPINGS.cbegin(); it != INPUT_MAPPINGS.end(); ++it) {
        DeviceEvent deviceEvent = it->first;
        if (deviceEvent.triggerType == TRIGGER_POSITION) {
            float axisValue = Gamepad::RAYLIB_MAPPINGS.at(deviceEvent.triggerType)(id, deviceEvent.trigger);
            GameEvent gameEvent = getGameEvent(deviceEvent);
            gameEvent.axisValue = axisValue;
            events.push_back(gameEvent);
        } else if (Gamepad::RAYLIB_MAPPINGS.at(deviceEvent.triggerType)(id, deviceEvent.trigger)) {
            events.push_back(getGameEvent(deviceEvent));
        }
    }
    return events;
}