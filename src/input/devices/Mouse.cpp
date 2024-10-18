//
// Created by felix-roehr on 5/4/24.
//

#include "Mouse.h"
#include <queue>

Mouse::Mouse(const InputConstants &inputConstants) : InputDevice(-1, DEVICE_MOUSE, inputConstants) {}
Mouse::~Mouse() = default;

std::queue<GameEvent> Mouse::getGameEvents() { return {}; }
