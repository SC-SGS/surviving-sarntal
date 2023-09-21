#pragma once

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};

struct AppInfo {
    bool isRunning = true;
};

struct Radius {
    float value;
};

struct Rock {};

struct CollidesWith {};

struct PossiblyCollidesWith {};
