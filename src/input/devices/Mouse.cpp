//
// Created by felix-roehr on 5/4/24.
//

#include "Mouse.h"
#include <queue>

Mouse::~Mouse() = default;

std::queue<GameEvent> Mouse::getGameEvents() { return {}; }
