#include "input.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

bool InputEntity::getGamepadEvent(ButtonEvent event, int gamepad) {
    switch (event.keyPressType) {
    case PRESSED:
        return IsGamepadButtonPressed(gamepad, event.key);
    case RELEASED:
        return IsGamepadButtonReleased(gamepad, event.key);
    case DOWN:
        return IsGamepadButtonDown(gamepad, event.key);
    case UP:
        return IsGamepadButtonUp(gamepad, event.key);
    default:
        return false;
    }
}

bool InputEntity::getKeyboardEvent(ButtonEvent event) {
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

double InputEntity::getGamepadAxis(GamepadAxis axis, int gamepad) {
    return GetGamepadAxisMovement(gamepad, axis);
}

double InputEntity::getVirtualAxis(VirtualAxis axis, int player) const {
    double value = 0;

    if (axis.positive.device == GAMEPAD && hasPlayerGamepad(player))
        value += getGamepadEvent(axis.positive, getPlayerGamepad(player));
    else if (axis.positive.device == KEYBOARD)
        value += getKeyboardEvent(axis.positive);

    if (axis.negative.device == GAMEPAD && hasPlayerGamepad(player))
        value -= getGamepadEvent(axis.negative, getPlayerGamepad(player));
    else if (axis.negative.device == KEYBOARD)
        value -= getKeyboardEvent(axis.negative);

    return value;
}

int InputEntity::getPlayerGamepad(int player) const {
    return player_gamepad_ids.at(player);
}

InputEntity::InputEntity() = default;
InputEntity::~InputEntity() = default;

bool InputEntity::getEvent(Event event, int player) const {
    bool active = false;

    if (hasPlayerGamepad(player)) {
        int gamepad = getPlayerGamepad(player);
        auto gamepad_action_iter = gamepad_key_map.find(event);
        if (gamepad_action_iter != gamepad_key_map.end()) {
            auto gamepad_action = gamepad_action_iter->second;
            active = active | getGamepadEvent(gamepad_action, gamepad);
        }
    }
    auto keyboard_action_iter = keyboard_key_map.find(event);
    if (keyboard_action_iter != keyboard_key_map.end()) {
        auto keyboard_action = keyboard_action_iter->second;
        active = active | getKeyboardEvent(keyboard_action);
    }

    return active;
}

double InputEntity::getAxis(Axis axis, int player) const {
    double value = 0;

    if (hasPlayerGamepad(player)) {
        int gamepad = getPlayerGamepad(player);
        auto gamepad_axis_iter = gamepad_axis_map.find(axis);
        if (gamepad_axis_iter != gamepad_axis_map.end()) {
            auto gamepad_axis = gamepad_axis_iter->second;
            value = getGamepadAxis(gamepad_axis, gamepad);
        }
    }
    auto virtual_axis_iter = virtual_axis_map.find(axis);
    if (virtual_axis_iter != virtual_axis_map.end()) {
        auto virtual_axes = virtual_axis_iter->second;

        // use value from virtual axis with maximal value
        for (auto virtual_axis : virtual_axes) {
            double virtual_value = getVirtualAxis(virtual_axis, player);
            if (abs(virtual_value) > abs(value))
                value = virtual_value;
        }
    }

    return value;
}

void InputEntity::preUpdate() {
    // if something must be updated each frame

    // update gamepads
    player_gamepad_ids.clear();
    for (int i = 0; i < 8; ++i) {
        // add to gamepads if its name contains
        // TODO: find more elegant way for this
        if (IsGamepadAvailable(i) &&
            std::string(GetGamepadName(i)).find("Gamepad") != std::string::npos)
            player_gamepad_ids.push_back(i);
    }
}

int InputEntity::getGamepadCount() const {
    return (int)player_gamepad_ids.size();
}

const std::string &InputEntity::getEventDisplayName(Event event) {
    return event_display_names.at(event);
}

const std::string &InputEntity::getAxisDisplayName(Axis axis) {
    return axis_display_names.at(axis);
}

std::string InputEntity::getInputInfo() const {
    std::stringstream input_info;

    for (int i = 0; i < Event::EVENT_COUNT; ++i) {
        input_info << getEventDisplayName((Event)i) << ": "
                   << getEvent((Event)i, 0) << std::endl;
    }

    for (int i = 0; i < Axis::AXIS_COUNT; ++i) {
        input_info << getAxisDisplayName((Axis)i) << ": " << getAxis((Axis)i, 0)
                   << std::endl;
    }

    return input_info.str();
}

bool InputEntity::hasPlayerGamepad(int player) const {
    return player < player_gamepad_ids.size();
}
