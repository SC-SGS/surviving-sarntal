//
// Created by Anietta Weckauff on 11.07.24.
//

#ifndef SURVIVING_SARNTAL_MENURENDERER_H
#define SURVIVING_SARNTAL_MENURENDERER_H

#include "../output/ResourceManager.h"
#include "MenuEnums.h"

class MenuRenderer {

  public:
    explicit MenuRenderer(ResourceManager &resourceManager);

    void draw();

    void drawStartScreen();

    void drawSettingsScreen();

    void drawInstructionsScreen();

    void drawEndScreen();

    void drawPauseScreen();

    void setScreenState(ScreenState newScreenState);

    ScreenState getScreenState();

    bool isGameplayRunning() const;

    void setGameplayRunning(bool gameplayRunning);

    bool isPlayAgainClicked() const;

    void setPlayAgainClicked(bool playAgainClicked);

  private:
    ResourceManager &resourceManager;
    bool gameplayRunning = false;
    bool playAgainClicked = false;
    ScreenState screenState = START_SCREEN;
    void checkMouseClickStartScreen();
    void checkMouseClickHomeButton();
    void checkMouseClickEndScreen();
};

#endif // SURVIVING_SARNTAL_MENURENDERER_H
