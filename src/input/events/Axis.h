//
// Created by felix-roehr on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_AXIS_H
#define SURVIVING_SARNTAL_AXIS_H

enum Axis {
    MOVEMENT_X,
    MOVEMENT_Y,
    SECONDARY_X,
    SECONDARY_Y,
    // one can switch to the next item or previous one
    ITEM_SWITCH,
    AXIS_COUNT, // can be used for iteration over axes
    NO_AXIS
};

#endif // SURVIVING_SARNTAL_AXIS_H
