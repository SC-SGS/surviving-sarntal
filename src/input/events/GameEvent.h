//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_GAMEEVENT_H
#define SURVIVING_SARNTAL_GAMEEVENT_H

#include "../../game/GameProperties.hpp"
#include "Axis.h"
#include "AxisModification.h"
#include "EventType.h"
#include <tuple>

/**
 * A game event has a type of event (e.g., CROUCH or MOVEMENT_X). Optionally, game
 * events also have an axis to be modified and a value that can be used to
 * modify the axis.
 */
struct GameEvent {
    EventType type;
    Axis axis;
    floatType axisValue;
    bool executeRepeatedly;

    // bool operator<(const GameEvent &e) const { return type < e.type || axis < e.axis; }
};

struct GameEventCompare {
    bool operator()(const GameEvent &lhs, const GameEvent &rhs) const {
        // Compare first trigger, then triggerType
        return std::tie(lhs.type, lhs.axis) < std::tie(rhs.type, rhs.axis);
    }
};

#endif // SURVIVING_SARNTAL_GAMEEVENT_H
