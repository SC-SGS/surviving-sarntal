#include "input.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

constexpr int MAX_GAMEPADS = 8;

/**
 * helper function to get the raylibs id for the i-th gamepad
 * @param gamepad_num
 * @return the id if found or NO_GAMEPAD_ID otherwise
 */
int findGamepadId(int gamepad_num) {
    int found_gamepads = 0;
    for (int i = 0; i < MAX_GAMEPADS; ++i) {
        // TODO: find more elegant way for this
        if (IsGamepadAvailable(i)) {
            std::string gamepad_name(GetGamepadName(i));
            if (IsGamepadAvailable(i) &&
                    gamepad_name.find("Gamepad") != std::string::npos ||
                gamepad_name.find("Controller") != std::string::npos ||
                gamepad_name.find("gamepad") != std::string::npos) {
                if (found_gamepads == gamepad_num)
                    return i;
                found_gamepads++;
            }
        }
    }

    return NO_GAMEPAD_ID;
}

bool InputEntity::getGamepadEvent(ButtonEvent event) const {
    switch (event.keyPressType) {
    case PRESSED:
        return IsGamepadButtonPressed(getGamepadId(), event.key);
    case RELEASED:
        return IsGamepadButtonReleased(getGamepadId(), event.key);
    case DOWN:
        return IsGamepadButtonDown(getGamepadId(), event.key);
    case UP:
        return IsGamepadButtonUp(getGamepadId(), event.key);
    default:
        return false;
    }
}

bool InputEntity::getKeyboardEvent(ButtonEvent event) const {
    switch (event.keyPressType) {
    case PRESSED:
        return IsKeyPressed(event.key);
    case RELEASED:
        return IsKeyReleased(event.key);
    case DOWN:
        return IsKeyDown(event.key);
    case UP:
        return IsKeyUp(event.key);
    default:
        return false;
    }
}

double InputEntity::getGamepadAxis(GamepadAxis axis) const {
    return GetGamepadAxisMovement(getGamepadId(), axis);
}

double InputEntity::getVirtualAxis(VirtualAxis axis) const {
    double value = 0;

    if (axis.positive.device == InputDevice::DEVICE_GAMEPAD && hasGamepad())
        value += getGamepadEvent(axis.positive);
    else if (axis.positive.device == InputDevice::DEVICE_KEYBOARD)
        value += getKeyboardEvent(axis.positive);

    if (axis.negative.device == InputDevice::DEVICE_GAMEPAD && hasGamepad())
        value -= getGamepadEvent(axis.negative);
    else if (axis.negative.device == DEVICE_KEYBOARD)
        value -= getKeyboardEvent(axis.negative);

    return value;
}

int InputEntity::getGamepadId() const { return gamepad_id; }

InputEntity::InputEntity() {
    if (getGamepadCount() > 0)
        setInputType(GAMEPAD, 0);
    else
        setInputType(MOUSE_KEYBOARD);
};
InputEntity::~InputEntity() = default;

bool InputEntity::getEvent(Event event) const {
    bool active = false;

    if (hasGamepad()) {
        auto gamepad_action_iter = gamepad_key_map.find(event);
        if (gamepad_action_iter != gamepad_key_map.end()) {
            auto gamepad_action = gamepad_action_iter->second;
            active = active | getGamepadEvent(gamepad_action);
        }
    }
    auto keyboard_action_iter = keyboard_key_map.find(event);
    if (keyboard_action_iter != keyboard_key_map.end()) {
        auto keyboard_action = keyboard_action_iter->second;
        active = active | getKeyboardEvent(keyboard_action);
    }

    return active;
}

double InputEntity::getAxis(Axis axis) const {
    double value = 0;

    if (hasGamepad()) {
        auto gamepad_axis_iter = gamepad_axis_map.find(axis);
        if (gamepad_axis_iter != gamepad_axis_map.end()) {
            auto gamepad_axis = gamepad_axis_iter->second;
            value = getGamepadAxis(gamepad_axis);
        }
    }
    auto virtual_axis_iter = virtual_axis_map.find(axis);
    if (virtual_axis_iter != virtual_axis_map.end()) {
        auto virtual_axes = virtual_axis_iter->second;

        // use value from virtual axis with maximal value
        for (auto virtual_axis : virtual_axes) {
            double virtual_value = getVirtualAxis(virtual_axis);
            if (abs(virtual_value) > abs(value))
                value = virtual_value;
        }
    }

    return value;
}

void InputEntity::preUpdate() {
    // if something must be updated each frame

    // update gamepad id
    gamepad_id = findGamepadId(gamepad_num);

    // use gamepad if present, keyboard otherwise
    if (gamepad_id != NO_GAMEPAD_ID)
        current_input_type = GAMEPAD;
    else
        current_input_type = MOUSE_KEYBOARD;
}

const std::string &InputEntity::getEventDisplayName(Event event) const {
    return event_display_names.at(event);
}

const std::string &InputEntity::getAxisDisplayName(Axis axis) const {
    return axis_display_names.at(axis);
}

std::string InputEntity::getInputInfo() const {
    std::stringstream input_info;

    for (int i = 0; i < Event::EVENT_COUNT; ++i) {
        input_info << getEventDisplayName((Event)i) << ": "
                   << getEvent((Event)i) << std::endl;
    }

    for (int i = 0; i < Axis::AXIS_COUNT; ++i) {
        input_info << getAxisDisplayName((Axis)i) << ": " << getAxis((Axis)i)
                   << std::endl;
    }

    return input_info.str();
}

bool InputEntity::hasGamepad() const {
    return current_input_type == InputType::GAMEPAD &&
           getGamepadId() != NO_GAMEPAD_ID;
}
void InputEntity::setInputType(InputEntity::InputType type, int _gamepad_num) {
    current_input_type = type;
    gamepad_num = _gamepad_num;
    gamepad_id = findGamepadId(_gamepad_num);
}
InputEntity::InputEntity(InputEntity::InputType input_type, int _gamepad_num) {
    setInputType(input_type, _gamepad_num);
}

bool InputEntity::getMouseEvent(ButtonEvent event) const {
    switch (event.keyPressType) {
    case PRESSED:
        return IsMouseButtonPressed(event.key);
    case RELEASED:
        return IsMouseButtonReleased(event.key);
    case DOWN:
        return IsMouseButtonDown(event.key);
    case UP:
        return IsMouseButtonUp(event.key);
    default:
        return false;
    }
}

int InputEntity::getGamepadCount() const {
    return findGamepadId(MAX_GAMEPADS);
}
