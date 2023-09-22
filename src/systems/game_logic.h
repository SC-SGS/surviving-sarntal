#pragma once

#include "flecs.h"

constexpr float KILL_BAR_VELOCITY = 100.;

constexpr int ROCK_TIME_PERIOD = 2;

void initGameLogic(flecs::world &);
