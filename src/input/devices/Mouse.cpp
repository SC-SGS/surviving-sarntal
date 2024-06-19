//
// Created by felix-roehr on 5/4/24.
//

#include "Mouse.h"
#include <queue>

// TODO magic mushrooms, erm numbers
Mouse::Mouse() : InputDevice(-1, DEVICE_MOUSE) {}
Mouse::~Mouse() = default;

std::queue<GameEvent> Mouse::getGameEvents() { return {}; }
