#ifndef SURVIVING_SARNTAL_INPUT_H
#define SURVIVING_SARNTAL_INPUT_H

#include "flecs.h"
#include "string"
#include <list>
#include <raylib.h>
#include <unordered_map>
#include <vector>

// Types of input devices
enum InputDevice { KEYBOARD, GAMEPAD, MOUSE, KINECT };

// In game events
enum Event {
    JUMP,
    DUCK,
    ITEM_PICK,
    ITEM_USE,
    ITEM_DROP,
    SPECIAL_ABILITY,
    PAUSE,
    EVENT_COUNT // can be used for iteration  over all events
};

// In game axes
enum Axis {
    MOVEMENT_X,
    MOVEMENT_Y,
    SECONDARY_X,
    SECONDARY_Y,
    // one can switch to the next item or previous one
    ITEM_SWITCH,
    AXIS_COUNT // can be used for iteration over axes
};

// when an event should be triggered (in the frame that it was pressed/released
// or always when a button is up/down
enum TriggerType { PRESSED, RELEASED, DOWN, UP };

// Hardware events (button presses on any input device)
struct ButtonEvent {
    InputDevice device;
    int key;
    TriggerType keyPressType;
};

// A virtual axis derived from two button events (one for positive values, one
// for negative ones)
struct VirtualAxis {
    ButtonEvent positive;
    ButtonEvent negative;
};

// map containing strings with the english display names for the input events
const std::unordered_map<Event, std::string> event_display_names{
    {JUMP, "Jump"},           {DUCK, "Duck"},
    {ITEM_PICK, "Pick item"}, {ITEM_USE, "Use item"},
    {ITEM_DROP, "Drop item"}, {SPECIAL_ABILITY, "Special ability"},
    {PAUSE, "Pause"},
};

// map containing the english display names of the relevant axes
const std::unordered_map<Axis, std::string> axis_display_names{
    {MOVEMENT_X, "Movement X"},
    {MOVEMENT_Y, "Movement Y"},
    {SECONDARY_X, "Secondary X"},
    {SECONDARY_Y, "Secondary Y"},
    {ITEM_SWITCH, "Switch Item"}};

const std::unordered_map<Event, ButtonEvent> keyboard_key_map{
    {JUMP, {KEYBOARD, KEY_SPACE, PRESSED}},
    {DUCK, {KEYBOARD, KEY_LEFT_CONTROL, PRESSED}},
    {ITEM_PICK, {KEYBOARD, KEY_E, PRESSED}},
    {ITEM_USE, {KEYBOARD, KEY_F, PRESSED}},
    {ITEM_DROP, {KEYBOARD, KEY_Q, PRESSED}},
    {SPECIAL_ABILITY, {KEYBOARD, KEY_G, PRESSED}},
    {PAUSE, {KEYBOARD, KEY_ESCAPE, PRESSED}}
    // add other entries
};

const std::unordered_map<Event, ButtonEvent> gamepad_key_map{
    {JUMP, {GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_DOWN, PRESSED}},
    {DUCK, {GAMEPAD, GAMEPAD_BUTTON_LEFT_TRIGGER_1, PRESSED}},
    {ITEM_PICK, {GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_LEFT, PRESSED}},
    {ITEM_USE, {GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, PRESSED}},
    {ITEM_DROP, {GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_UP, PRESSED}},
    {SPECIAL_ABILITY, {GAMEPAD, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, PRESSED}},
    {PAUSE, {GAMEPAD, GAMEPAD_BUTTON_MIDDLE, PRESSED}},
};

const std::unordered_map<Axis, GamepadAxis> gamepad_axis_map{
    {MOVEMENT_X, {GAMEPAD_AXIS_LEFT_X}},
    {MOVEMENT_Y, {GAMEPAD_AXIS_LEFT_Y}},
    {SECONDARY_X, {GAMEPAD_AXIS_RIGHT_X}},
    {SECONDARY_Y, {GAMEPAD_AXIS_RIGHT_Y}}};

// a map that stores sources for virtual axes (axes derived from key presses)
const std::unordered_map<Axis, std::list<VirtualAxis>> virtual_axis_map{
    {ITEM_SWITCH,
     {{GAMEPAD, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, DOWN},
      {GAMEPAD, GAMEPAD_BUTTON_LEFT_TRIGGER_1, DOWN}}},
    {MOVEMENT_X,
     {
         {{KEYBOARD, KEY_D, DOWN}, {KEYBOARD, KEY_A, DOWN}},
         {{GAMEPAD, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, DOWN},
          {GAMEPAD, GAMEPAD_BUTTON_LEFT_FACE_LEFT, DOWN}},
     }},

};

class InputEntity {
  private:
    static bool getGamepadEvent(ButtonEvent event, int gamepad);

    static bool getKeyboardEvent(ButtonEvent event);

    static double getGamepadAxis(GamepadAxis axis, int gamepad);

    int getPlayerGamepad(int player) const;

    bool hasPlayerGamepad(int player) const;

    double getVirtualAxis(VirtualAxis axis, int player) const;

    // a list of the ids of attached gamepads
    std::vector<int> player_gamepad_ids;

  public:
    InputEntity();

    ~InputEntity();

    static const std::string &getAxisDisplayName(Axis axis);

    static const std::string &getEventDisplayName(Event event);

    std::string getInputInfo() const;

    // gets the number of attached gamepads
    int getGamepadCount() const;

    // checks if an event (usually pressing of a button) is present at the
    // moment
    bool getEvent(Event event, int player = 0) const;

    // gets the value for an axis (either joystick or virtual axis derived from
    // button events)
    double getAxis(Axis axis, int player = 0) const;

    void preUpdate();
};

#endif // SURVIVING_SARNTAL_INPUT_H
