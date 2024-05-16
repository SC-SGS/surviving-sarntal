//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_GAMEEVENT_H
#define SURVIVING_SARNTAL_GAMEEVENT_H

#include "Axis.h"
#include "AxisModification.h"
#include "EventType.h"

/**
 * A game event has a type of event (e.g., DUCK or MOVEMENT_X). Optionally, game
 * events also have an axis to be modified and a value that can be used to
 * modify the axis.
 */
struct GameEvent {
    EventType type;
    Axis axis;
    float axisValue;
};

#endif // SURVIVING_SARNTAL_GAMEEVENT_H
