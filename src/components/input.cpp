#include "input.h"

#include "../utils/kinect_variables.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "kinect_handler.h"

constexpr int MAX_GAMEPADS = 8;
double horizontal_axis = 0.0;
bool do_kinect_jump = false;
bool do_kinect_duck = false;

/**
 * checks if the device with the given id (in raylib) is a valid gamepad.
 * checks if the name of the device matches some keywords (Gamepad, Controller)
 * @param raylib_id
 * @return
 */
bool isGamepad(int raylib_id) {
    if (!IsGamepadAvailable(raylib_id))
        return false;

    std::string gamepad_name(GetGamepadName(raylib_id));
    bool result = gamepad_name.find("Gamepad") != std::string::npos ||
                  gamepad_name.find("Controller") != std::string::npos ||
                  gamepad_name.find("gamepad") != std::string::npos;
    return result;
}

/**
 * gets the number of valid gamepads that can be accessed via raylib
 * @return
 */
int InputEntity::getGamepadCount() {
    int found_gamepads = 0;
    for (int i = 0; i < MAX_GAMEPADS; ++i) {
        if (isGamepad(i))
            found_gamepads++;
    }
    return found_gamepads;
}

/**
 * helper function to get raylibs id for the i-th gamepad
 * @param gamepad_num
 * @return the id if found or NO_GAMEPAD_ID otherwise
 */
int findRaylibId(int gamepad_num) {
    int found_gamepads = 0;
    for (int i = 0; i < MAX_GAMEPADS; ++i) {
        if (isGamepad(i)) {
            if (found_gamepads == gamepad_num)
                return i;
            found_gamepads++;
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

int InputEntity::getGamepadId() const { return gamepad_id_raylib; }

bool InputEntity::getEvent(Event event) const {
    bool active = false;

    if (current_input_type == DISABLED) {
        return active;
    } else if (current_input_type == GAMEPAD && hasGamepad()) {
        auto gamepad_action_iter = GAMEPAD_KEY_MAP.find(event);
        if (gamepad_action_iter != GAMEPAD_KEY_MAP.end()) {
            auto gamepad_action = gamepad_action_iter->second;
            active = getGamepadEvent(gamepad_action);
        }
    } else if (current_input_type == KINECT && hasKinect()) {
        if (event == JUMP) {
            active = do_kinect_jump;
        } else if (event == DUCK) {
            active = do_kinect_duck;
        }
    } else if (current_input_type == MOUSE_KEYBOARD) {
        auto keyboard_action_iter = KEYBOARD_KEY_MAP.find(event);
        if (keyboard_action_iter != KEYBOARD_KEY_MAP.end()) {
            auto keyboard_action = keyboard_action_iter->second;
            active = getKeyboardEvent(keyboard_action);
        }
    }

    return active;
}

bool InputEntity::useVirtualAxis(VirtualAxis axis) const {
    switch (current_input_type) {
    case GAMEPAD:
        return axis.positive.device == DEVICE_GAMEPAD ||
               axis.negative.device == DEVICE_GAMEPAD;
    case KINECT:
        return axis.positive.device == DEVICE_KINECT ||
               axis.negative.device == DEVICE_KINECT;
    case MOUSE_KEYBOARD:
        return axis.positive.device == DEVICE_KEYBOARD ||
               axis.negative.device == DEVICE_KEYBOARD ||
               axis.positive.device == DEVICE_MOUSE ||
               axis.negative.device == DEVICE_MOUSE;
    default:
        return false;
    }
}

double InputEntity::getAxis(Axis axis) const {
    double value = 0;

    if (current_input_type == DISABLED) {
        return value;
    } else if (current_input_type == GAMEPAD && hasGamepad()) {
        auto gamepad_axis_iter = GAMEPAD_AXIS_MAP.find(axis);
        if (gamepad_axis_iter != GAMEPAD_AXIS_MAP.end()) {
            auto gamepad_axis = gamepad_axis_iter->second;
            value = getGamepadAxis(gamepad_axis);
        }
    } else if (current_input_type == KINECT) {
        value = horizontal_axis;
    }

    auto virtual_axis_iter = VIRTUAL_AXIS_MAP.find(axis);
    if (virtual_axis_iter != VIRTUAL_AXIS_MAP.end()) {
        auto virtual_axes = virtual_axis_iter->second;

        // use value from relevant virtual axes with maximal value
        for (auto virtual_axis : virtual_axes) {
            double virtual_value = getVirtualAxis(virtual_axis);
            if (useVirtualAxis(virtual_axis) && abs(virtual_value) > abs(value))
                value = virtual_value;
        }
    }
    return value;
}

void InputEntity::updateDevices() {
    // if something must be updated each frame

    gamepad_id_raylib = findRaylibId(gamepad_num);

    if (auto_select_device) {
        if (hasKinect() && hasGamepad()) {
            // current_input_type = KINECTPAD;
        } else if (hasGamepad()) {
            current_input_type = GAMEPAD;
        } else if (hasKinect()) {
            current_input_type = KINECT;
        } else {
            current_input_type = MOUSE_KEYBOARD;
        }
    }
}

const std::string &InputEntity::getEventDisplayName(Event event) {
    return EVENT_DISPLAY_NAMES.at(event);
}

const std::string &InputEntity::getAxisDisplayName(Axis axis) {
    return AXIS_DISPLAY_NAMES.at(axis);
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

bool InputEntity::hasGamepad() const { return getGamepadId() != NO_GAMEPAD_ID; }
void InputEntity::setInputType(InputEntity::InputType type, int _gamepad_num) {
    current_input_type = type;
    gamepad_num = _gamepad_num;
    gamepad_id_raylib = findRaylibId(_gamepad_num);
}

InputEntity::InputEntity() {
    auto_select_device = true;
    setInputType(MOUSE_KEYBOARD, 0);
}
InputEntity::InputEntity(InputEntity::InputType input_type, int _gamepad_num) {
    auto_select_device = false;
    setInputType(input_type, _gamepad_num);
}

bool InputEntity::getMouseEvent(ButtonEvent event) {
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
InputEntity::InputType InputEntity::getInputType() const {
    return current_input_type;
}

void rumbleThread(SDL_GameController *controller, int strength, int duration) {
    // Set rumble intensity for both motors (left and right)
    SDL_GameControllerRumble(controller, strength, strength,
                             duration); // Values range from 0 to 65535

    // Sleep for the specified duration
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));

    // Stop the rumble effect
    SDL_GameControllerRumble(controller, 0, 0, 0);
}

void InputEntity::rumble(int strength, int duration) const {
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL initialization error: " << SDL_GetError()
                  << std::endl;
        return;
    }

    SDL_GameController *controller = SDL_GameControllerOpen(getGamepadId());

    if (!controller) {
        //std::cerr << "No game controller found!" << std::endl;
        SDL_Quit();
        return;
    }

    // Create a thread to run the rumble effect
    std::thread rumbleThreadObj(rumbleThread, controller, strength, duration);

    // Detach the thread to allow it to run independently
    rumbleThreadObj.detach();

    // Continue with other tasks or return immediately without blocking
}

bool InputEntity::hasKinect() const { return kinect_mode; }
int InputEntity::getDeviceNum() const { return gamepad_num; }