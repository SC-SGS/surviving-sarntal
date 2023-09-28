#pragma once

#include "../components/vector.h"
#include "flecs.h"

void initGameLogic(flecs::world &);

void spawnExplosion(flecs::world &world, Position pos);