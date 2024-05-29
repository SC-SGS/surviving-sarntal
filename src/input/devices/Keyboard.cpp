//
// Created by felix-roehr on 5/4/24.
//

#include "Keyboard.h"
#include "raylib.h"

Keyboard::Keyboard() {
    INPUT_MAPPINGS = {// Normal events
                      {{KEY_SPACE, TRIGGER_PRESSED}, {JUMP, NO_AXIS, 0}},
                      {{KEY_LEFT_SHIFT, TRIGGER_DOWN}, {CROUCH, NO_AXIS, 0}},
                      {{KEY_LEFT_SHIFT, TRIGGER_RELEASED}, {UNCROUCH, NO_AXIS, 0}},
                      {{KEY_Q, TRIGGER_PRESSED}, {ITEM_PICK, NO_AXIS, 0}},
                      {{KEY_E, TRIGGER_PRESSED}, {ITEM_USE, NO_AXIS, 0}},
                      {{KEY_F, TRIGGER_PRESSED}, {ITEM_DROP, NO_AXIS, 0}},
                      {{KEY_R, TRIGGER_PRESSED}, {SPECIAL_ABILITY, NO_AXIS, 0}},
                      {{KEY_F11, TRIGGER_PRESSED}, {FULLSCREEN, NO_AXIS, 0}},
                      {{KEY_P, TRIGGER_PRESSED}, {TOGGLE_DEBUG, NO_AXIS, 0}},
                      {{KEY_BACKSPACE, TRIGGER_PRESSED}, {PAUSE, NO_AXIS, 0}},

                      // Virtual axis modifications
                      {{KEY_TAB, TRIGGER_PRESSED}, {AXIS_MODIFICATION, ITEM_SWITCH, 1}},
                      {{KEY_D, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, 1}},
                      {{KEY_A, TRIGGER_DOWN}, {AXIS_MODIFICATION, MOVEMENT_X, -1}}};
}

Keyboard::~Keyboard() = default;

const std::map<TriggerType, std::function<bool(int)>> Keyboard::RAYLIB_MAPPINGS{{TRIGGER_PRESSED, IsKeyPressed},
                                                                                {TRIGGER_DOWN, IsKeyDown},
                                                                                {TRIGGER_RELEASED, IsKeyReleased},
                                                                                {TRIGGER_UP, IsKeyUp}};

std::list<GameEvent> Keyboard::getGameEvents() {
    std::list<GameEvent> events = {};
    for (auto it = INPUT_MAPPINGS.cbegin(); it != INPUT_MAPPINGS.end(); ++it) {
        DeviceEvent deviceEvent = it->first;
        if (Keyboard::RAYLIB_MAPPINGS.at(deviceEvent.triggerType)(deviceEvent.trigger)) {
            events.push_back(getGameEvent(deviceEvent));
        }
    }
    return events;
}
