#pragma once

#include "flecs.h"
#include "../components/vector.h"

void initGameLogic(flecs::world &);

void spawnExplosion(flecs::world &world, Position pos);