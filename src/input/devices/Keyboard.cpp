//
// Created by felix-roehr on 5/4/24.
//

#include "Keyboard.h"
#include "raylib.h"
#include "spdlog/spdlog.h"
#include <iostream>

Keyboard::Keyboard(const InputConstants &inputConstants) : InputDevice(-1, DEVICE_KEYBOARD, inputConstants) {
    INPUT_MAPPINGS = {// Normal events
                      {{KEY_SPACE, TRIGGER_PRESSED}, {JUMP, NO_AXIS, 0, false}},
                      {{KEY_LEFT_SHIFT, TRIGGER_DOWN}, {CROUCH, NO_AXIS, 0, false}},
                      {{KEY_LEFT_SHIFT, TRIGGER_RELEASED}, {UNCROUCH, NO_AXIS, 0, false}},
                      {{KEY_Q, TRIGGER_PRESSED}, {ITEM_PICK, NO_AXIS, 0, false}},
                      {{KEY_E, TRIGGER_PRESSED}, {ITEM_USE, NO_AXIS, 0, false}},
                      {{KEY_F, TRIGGER_PRESSED}, {ITEM_DROP, NO_AXIS, 0, false}},
                      {{KEY_R, TRIGGER_PRESSED}, {SPECIAL_ABILITY, NO_AXIS, 0, false}},
                      {{KEY_F11, TRIGGER_PRESSED}, {FULLSCREEN, NO_AXIS, 0, false}},
                      {{KEY_P, TRIGGER_PRESSED}, {TOGGLE_DEBUG, NO_AXIS, 0, false}},
                      {{KEY_BACKSPACE, TRIGGER_PRESSED}, {PAUSE, NO_AXIS, 0, false}},
                      {{KEY_ENTER, TRIGGER_PRESSED}, {SELECT_BUTTON, NO_AXIS, 0, false}},
                      //{{KEY_S, TRIGGER_PRESSED}, {CONTROLS, NO_AXIS, 0, false}},
                      //{{KEY_I, TRIGGER_PRESSED}, {INSTRUCTIONS, NO_AXIS, 0, false}},
                      //{{KEY_M, TRIGGER_PRESSED}, {MENU, NO_AXIS, 0, false}},

                      // Virtual axis modifications
                      {{KEY_RIGHT, TRIGGER_PRESSED}, {AXIS_MODIFICATION, BUTTON_SWITCH, 1, false}},
                      {{KEY_LEFT, TRIGGER_PRESSED}, {AXIS_MODIFICATION, BUTTON_SWITCH, -1, false}},

                      {{KEY_TAB, TRIGGER_PRESSED}, {AXIS_MODIFICATION, ITEM_SWITCH, 1, false}},
                      {{KEY_D, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, 1, true}},
                      {{KEY_A, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, -1, true}},
                      {{KEY_D, TRIGGER_RELEASED}, {AXIS_MODIFICATION, MOVEMENT_X, 0, false}},
                      {{KEY_A, TRIGGER_RELEASED}, {AXIS_MODIFICATION, MOVEMENT_X, 0, false}}};
}

Keyboard::~Keyboard() = default;

const std::map<TriggerType, std::function<bool(int)>> Keyboard::raylibMappings{{TRIGGER_PRESSED, IsKeyPressed},
                                                                               {TRIGGER_DOWN, IsKeyDown},
                                                                               {TRIGGER_RELEASED, IsKeyReleased},
                                                                               {TRIGGER_UP, IsKeyUp}};

std::queue<GameEvent> Keyboard::getGameEvents() {
    std::queue<GameEvent> events = {};
    for (auto it = INPUT_MAPPINGS.cbegin(); it != INPUT_MAPPINGS.end(); ++it) {
        DeviceEvent deviceEvent = it->first;
        if (Keyboard::raylibMappings.at(deviceEvent.triggerType)(deviceEvent.trigger)) {
            spdlog::debug("Device Event: {}", it->first.trigger);
            events.push(getGameEvent(deviceEvent));
        }
    }
    return events;
}
