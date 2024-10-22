//
// Created by Aleksis Vezenkov on 23.09.24.
//

#ifndef SURVIVING_SARNTAL_DIFFICULTYSERVICE_HPP
#define SURVIVING_SARNTAL_DIFFICULTYSERVICE_HPP

#include "../utilities/Singleton.hpp"
#include "GameProperties.hpp"
class DifficultyService : public Singleton<DifficultyService> {
    friend class Singleton<DifficultyService>;

  public:
    int getCurrentDifficultyLevel();
    void startGamePlay();
    void reset();
    void setGameConstants(GameConstants constants);
    double getElapsedGamePlayTime();

  private:
    double gamePlayStart = 0.0;
    ~DifficultyService();
    DifficultyService();
    GameConstants gameConstants;
    int calculateLevelEndingPoint(int difficultyLevel) const;
};

#endif // SURVIVING_SARNTAL_DIFFICULTYSERVICE_HPP
