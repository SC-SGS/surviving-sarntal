//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_DEVICEEVENT_H
#define SURVIVING_SARNTAL_DEVICEEVENT_H

#include "TriggerType.h"
#include <tuple>

/**
 * A device event includes the trigger that caused the event and the type of
 * trigger (e.g., pressed or position).
 */
struct DeviceEvent {
    int trigger;
    TriggerType triggerType;

    // bool operator<(const DeviceEvent &e) const { return trigger < e.trigger || triggerType < e.triggerType; }
};

struct DeviceEventCompare {
    bool operator()(const DeviceEvent &lhs, const DeviceEvent &rhs) const {
        // Compare first trigger, then triggerType
        return std::tie(lhs.trigger, lhs.triggerType) < std::tie(rhs.trigger, rhs.triggerType);
    }
};

#endif // SURVIVING_SARNTAL_DEVICEEVENT_H
