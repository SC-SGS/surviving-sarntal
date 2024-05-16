//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_EVENTTYPE_H
#define SURVIVING_SARNTAL_EVENTTYPE_H

enum EventType {
    JUMP,
    DUCK,
    ITEM_PICK,
    ITEM_USE,
    ITEM_DROP,
    SPECIAL_ABILITY,
    PAUSE,
    FULLSCREEN,
    TOGGLE_DEBUG,
    // EVENT_COUNT, // can be used for iteration  over all events
    AXIS_MODIFICATION,
    NO_EVENT
};

#endif // SURVIVING_SARNTAL_EVENTTYPE_H
