#ifndef SURVIVING_SARNTAL_INPUT_SYSTEMS_H
#define SURVIVING_SARNTAL_INPUT_SYSTEMS_H

#include "../components/input.h"
#include "flecs.h"
#include "string"

struct InputSystems {
    explicit InputSystems(flecs::world &world);
};

#endif // SURVIVING_SARNTAL_INPUT_SYSTEMS_H
