//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_EVENTTYPE_H
#define SURVIVING_SARNTAL_EVENTTYPE_H

enum EventType {
    JUMP,
    CROUCH,
    UNCROUCH,
    ITEM_PICK,
    ITEM_USE,
    ITEM_DROP,
    SPECIAL_ABILITY,
    PAUSE,
    FULLSCREEN,
    TOGGLE_DEBUG,
    // EVENT_COUNT, // can be used for iteration  over all events
    AXIS_MODIFICATION,
    NO_EVENT,
    SELECT_BUTTON
};

#endif // SURVIVING_SARNTAL_EVENTTYPE_H
