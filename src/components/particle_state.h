#pragma once

struct AppInfo {
    bool isRunning = true;
    bool playerAlive = true;
    bool playedDeadSound = false;
    int score{0};
    int coin_score{0};
};
