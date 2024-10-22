//
// Created by Aleksis Vezenkov on 23.09.24.
//

#include "DifficultyService.hpp"

#include <utility>

#include "raylib.h"

int DifficultyService::getCurrentDifficultyLevel() {
    auto elapsedTime = this->getElapsedGamePlayTime();
    int highestDiffLevel = this->gameConstants.difficultyConstants.highestDifficultyLevel;

    for (int difficultyLevel = 0; difficultyLevel < highestDiffLevel; difficultyLevel++) {
        double levelEndingTime = this->calculateLevelEndingPoint(difficultyLevel);
        if (elapsedTime < levelEndingTime) {
            return difficultyLevel;
        }
    }
    return highestDiffLevel;
}

int DifficultyService::calculateLevelEndingPoint(int difficultyLevel) const {
    int factor = this->gameConstants.difficultyConstants.difficultyFactor;
    int startingPoint = this->gameConstants.difficultyConstants.difficultyStartingPoint;
    return (factor * difficultyLevel) + startingPoint;
}

void DifficultyService::setGameConstants(GameConstants constants) { this->gameConstants = std::move(constants); }

void DifficultyService::reset() { this->gamePlayStart = 0.0; }

double DifficultyService::getElapsedGamePlayTime() {
    if (this->gamePlayStart == 0.0) {
        this->gamePlayStart = GetTime();
        return 0.0;
    } else {
        return GetTime() - this->gamePlayStart;
    }
}
void DifficultyService::startGamePlay() { this->reset(); }
DifficultyService::~DifficultyService() = default;
DifficultyService::DifficultyService() = default;
