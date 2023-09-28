#ifndef SURVIVING_SARNTAL_INPUT_H
#define SURVIVING_SARNTAL_INPUT_H

#include "flecs.h"
#include "list"
#include "raylib.h"
#include "string"
#include "unordered_map"
#include "vector"

static constexpr int NO_GAMEPAD_ID = -1;

// different input devices
enum InputDevice {
    DEVICE_KEYBOARD,
    DEVICE_GAMEPAD,
    DEVICE_MOUSE,
    DEVICE_KINECT
};

// In game events
enum Event {
    JUMP,
    DUCK,
    ITEM_PICK,
    ITEM_USE,
    ITEM_DROP,
    SPECIAL_ABILITY,
    PAUSE,
    FULLSCREEN,
    TOGGLE_DEBUG,
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

// when an event should be triggered (in the frame that it was
// pressed/released or always when a button is up/down
enum TriggerType { PRESSED, RELEASED, DOWN, UP };

// Hardware events (button presses on any input device)
struct ButtonEvent {
    InputDevice device;
    int key;
    TriggerType keyPressType;
};

// A virtual axis derived from two button events (one for positive values,
// one for negative ones)
struct VirtualAxis {
    ButtonEvent positive;
    ButtonEvent negative;
};

// map containing strings with the english display names for the input
// events
const std::unordered_map<Event, std::string> EVENT_DISPLAY_NAMES{
    {JUMP, "Jump"},           {DUCK, "Ducking"},
    {ITEM_PICK, "Pick item"}, {ITEM_USE, "Use item"},
    {ITEM_DROP, "Drop item"}, {SPECIAL_ABILITY, "Special ability"},
    {PAUSE, "Pause"},
};

// map containing the english display names of the relevant axes
const std::unordered_map<Axis, std::string> AXIS_DISPLAY_NAMES{
    {MOVEMENT_X, "Movement X"},
    {MOVEMENT_Y, "Movement Y"},
    {SECONDARY_X, "Secondary X"},
    {SECONDARY_Y, "Secondary Y"},
    {ITEM_SWITCH, "Switch Item"}};

const std::unordered_map<Event, ButtonEvent> KEYBOARD_KEY_MAP{
    {JUMP, {DEVICE_KEYBOARD, KEY_SPACE, PRESSED}},
    {DUCK, {DEVICE_KEYBOARD, KEY_LEFT_SHIFT, DOWN}},
    {ITEM_PICK, {DEVICE_KEYBOARD, KEY_E, PRESSED}},
    {ITEM_USE, {DEVICE_KEYBOARD, KEY_F, PRESSED}},
    {ITEM_DROP, {DEVICE_KEYBOARD, KEY_Q, PRESSED}},
    {SPECIAL_ABILITY, {DEVICE_KEYBOARD, KEY_G, PRESSED}},
    {FULLSCREEN, {DEVICE_KEYBOARD, KEY_F11, PRESSED}},
    {TOGGLE_DEBUG, {DEVICE_KEYBOARD, KEY_P, PRESSED}},
    {PAUSE, {DEVICE_KEYBOARD, KEY_ESCAPE, PRESSED}}};

const std::unordered_map<Event, ButtonEvent> GAMEPAD_KEY_MAP{
    {JUMP, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_DOWN, PRESSED}},
    {DUCK, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_TRIGGER_1, DOWN}},
    {ITEM_PICK, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_LEFT, PRESSED}},
    {ITEM_USE, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, PRESSED}},
    {ITEM_DROP, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_UP, PRESSED}},
    {SPECIAL_ABILITY,
     {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, PRESSED}},
    {PAUSE, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_MIDDLE, PRESSED}},
};

const std::unordered_map<Axis, GamepadAxis> GAMEPAD_AXIS_MAP{
    {MOVEMENT_X, {GAMEPAD_AXIS_LEFT_X}},
    {MOVEMENT_Y, {GAMEPAD_AXIS_LEFT_Y}},
    {SECONDARY_X, {GAMEPAD_AXIS_RIGHT_X}},
    {SECONDARY_Y, {GAMEPAD_AXIS_RIGHT_Y}}};

// a map that stores sources for virtual axes (axes derived from key
// presses)
const std::unordered_map<Axis, std::list<VirtualAxis>> VIRTUAL_AXIS_MAP{
    {
        ITEM_SWITCH,
        {{DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, PRESSED},
         {DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_TRIGGER_1, PRESSED},
         {{DEVICE_KEYBOARD, KEY_M, PRESSED},
          {DEVICE_KEYBOARD, KEY_N, PRESSED}}},
    },
    {MOVEMENT_X,
     {
         {{DEVICE_KEYBOARD, KEY_D, DOWN}, {DEVICE_KEYBOARD, KEY_A, DOWN}},
         {{DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, DOWN},
          {DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_FACE_LEFT, DOWN}},
     }},
};

class InputEntity {
  public:
    enum InputType { DISABLED, GAMEPAD, MOUSE_KEYBOARD, KINECT };

  private:
    // whether to automatically select the best input device on update
    bool auto_select_device = false;

    InputType current_input_type = DISABLED;
    // number of the gamepad to use (if input type is Gamepad)
    int gamepad_num = 0;
    // the gamepad id in raylib (if input type is Gamepad)
    int gamepad_id_raylib = NO_GAMEPAD_ID;

    /**
     * checks for button press events on the currently selected gamepad
     * @param event
     * @return
     */
    bool getGamepadEvent(ButtonEvent event) const;

    /**
     * checks for keypress events on the keyboard
     * @param event
     * @return
     */
    static bool getKeyboardEvent(ButtonEvent event);

    /**
     * checks for mouse button events
     * @param event
     * @return
     */
    static bool getMouseEvent(ButtonEvent event);

    /**
     * reads the value of a gamepad axis (stick or trigger buttons)
     * @param axis
     * @return
     */
    double getGamepadAxis(GamepadAxis axis) const;

    /**
     * gets the raylib id for the selected gamepad
     * @return
     */
    int getGamepadId() const;

    /**
     * checks if the gamepad is selected for input and is available
     * @return
     */
    bool hasGamepad() const;

    /**
     * checks if kinect is selected for input and available
     * @return
     */
    bool hasKinect() const;

    /**
     * checks if a virtual axis should be used with the currently selected input
     * method.
     * @param axis
     * @return
     */
    bool useVirtualAxis(VirtualAxis axis) const;
    /**
     * reads the value of a virtual axis (i.e. an axis derived from two buttons)
     * @param axis
     * @return
     */
    double getVirtualAxis(VirtualAxis axis) const;

  public:
    /**
     * constructor for an InputEntity that automatically selects an input device
     */
    explicit InputEntity();

    /**
     * a constructor that allows to specify the input type and optionally the
     * number of the gamepad. This input type is kept even if the device(s)
     * are not available.
     * @param input_type
     * @param gamepad_num
     */
    explicit InputEntity(InputType input_type, int gamepad_num = NO_GAMEPAD_ID);

    static const std::string &getAxisDisplayName(Axis axis);

    static const std::string &getEventDisplayName(Event event);

    /**
     * gets formatted info about currently provided events and axis inputs
     * @return
     */
    std::string getInputInfo() const;

    /**
     * Sets the input type of this InputEntity to the given one.
     * One can choose between mouse/keyboard, gamepad and kinect.
     * When selecting the gamepad, the number of the gamepad has to be specified
     * too
     * @param type
     * @param gamepad_num
     */
    void setInputType(InputType type, int gamepad_num = NO_GAMEPAD_ID);

    /**
     * gets the currently selected input type
     */
    InputType getInputType() const;
    /**
     * gets the number of the gamepad
     * @return
     */
    int getDeviceNum() const;

    /**
     * gets the number of gamepads that are attached
     * (filtering out devices that are wrongly recognized by raylib, like
     * touchpads)
     * @return the number of gamepads
     */
    static int getGamepadCount();

    /**
     * checks if an event (usually pressing of a button) is present at the
     * moment. only checks input devices associated with this input entity
     * @param event which event to check, {@link Event}
     * @return true if the event is provided in this frame, false otherwise
     */
    bool getEvent(Event event) const;

    /**
     * gets the scalar value for an axis (either joystick or virtual axis
     * derived from button events). only checks input devices associated with
     * this input entity
     * @param axis
     * @return
     */
    double getAxis(Axis axis) const;

    /**
     * checks that the selected device is still available
     */
    void updateDevices();
};

#endif // SURVIVING_SARNTAL_INPUT_H
