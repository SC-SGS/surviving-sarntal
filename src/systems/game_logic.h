#pragma once

#include "flecs.h"

constexpr float KILL_BAR_VELOCITY = 100.;

constexpr float ROCK_TIME_PERIOD_DIFFICULT = 1;
constexpr float ROCK_TIME_PERIOD_MEDIUM = 1.5;
constexpr float ROCK_TIME_PERIOD_EASY = 2;

void initGameLogic(flecs::world &);
