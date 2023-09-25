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
    {JUMP, {DEVICE_KEYBOARD, KEY_SPACE, PRESSED}},
    {DUCK, {DEVICE_KEYBOARD, KEY_LEFT_CONTROL, DOWN}},
    {ITEM_PICK, {DEVICE_KEYBOARD, KEY_E, PRESSED}},
    {ITEM_USE, {DEVICE_KEYBOARD, KEY_F, PRESSED}},
    {ITEM_DROP, {DEVICE_KEYBOARD, KEY_Q, PRESSED}},
    {SPECIAL_ABILITY, {DEVICE_KEYBOARD, KEY_G, PRESSED}},
    {PAUSE, {DEVICE_KEYBOARD, KEY_ESCAPE, PRESSED}}};

const std::unordered_map<Event, ButtonEvent> gamepad_key_map{
    {JUMP, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_DOWN, PRESSED}},
    {DUCK, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_TRIGGER_1, PRESSED}},
    {ITEM_PICK, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_LEFT, PRESSED}},
    {ITEM_USE, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, PRESSED}},
    {ITEM_DROP, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_FACE_UP, PRESSED}},
    {SPECIAL_ABILITY,
     {DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, PRESSED}},
    {PAUSE, {DEVICE_GAMEPAD, GAMEPAD_BUTTON_MIDDLE, PRESSED}},
};

const std::unordered_map<Axis, GamepadAxis> gamepad_axis_map{
    {MOVEMENT_X, {GAMEPAD_AXIS_LEFT_X}},
    {MOVEMENT_Y, {GAMEPAD_AXIS_LEFT_Y}},
    {SECONDARY_X, {GAMEPAD_AXIS_RIGHT_X}},
    {SECONDARY_Y, {GAMEPAD_AXIS_RIGHT_Y}}};

// a map that stores sources for virtual axes (axes derived from key
// presses)
const std::unordered_map<Axis, std::list<VirtualAxis>> virtual_axis_map{
    {ITEM_SWITCH,
     {{DEVICE_GAMEPAD, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, DOWN},
      {DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_TRIGGER_1, DOWN}}},
    {MOVEMENT_X,
     {
         {{DEVICE_KEYBOARD, KEY_D, DOWN}, {DEVICE_KEYBOARD, KEY_A, DOWN}},
         {{DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, DOWN},
          {DEVICE_GAMEPAD, GAMEPAD_BUTTON_LEFT_FACE_LEFT, DOWN}},
     }},
};

class InputEntity {
  public:
    enum InputType { GAMEPAD, MOUSE_KEYBOARD, KINECT };

  private:
    InputType current_input_type;
    // number of the gamepad to use (if input type is Gamepad)
    int gamepad_num;
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
    bool getKeyboardEvent(ButtonEvent event) const;

    /**
     * checks for mouse button events
     * @param event
     * @return
     */
    bool getMouseEvent(ButtonEvent event) const;

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
     * default constructor that selects the first gamepad if available
     * or mouse and keyboard otherwise
     */
    InputEntity();

    /**
     * a constructor that allows to specify the input type and optionally the
     * number of the gamepad
     * @param input_type
     * @param gamepad_num
     */
    InputEntity(InputType input_type, int gamepad_num = NO_GAMEPAD_ID);

    ~InputEntity();

    const std::string &getAxisDisplayName(Axis axis) const;

    const std::string &getEventDisplayName(Event event) const;

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
     * gets the number of gamepads that are attached
     * (filtering out devices that are wrongly recognized by raylib, like
     * touchpads)
     * @return the number of gamepads
     */
    int getGamepadCount() const;

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
