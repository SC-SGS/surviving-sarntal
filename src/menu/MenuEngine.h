//
// Created by Anietta Weckauff on 26.07.24.
//

#ifndef SURVIVING_SARNTAL_MENUENGINE_H
#define SURVIVING_SARNTAL_MENUENGINE_H

#include "screens/EndScreen.h"
#include "screens/InstructionsScreen.h"
#include "screens/PauseScreen.h"
#include "screens/Screen.h"
#include "screens/SettingsScreen.h"
#include "screens/StartScreen.h"

class MenuEngine {
  private:
    ResourceManager &resourceManager;
    ScreenState currentScreen;
    bool gameplayRunning;
    bool playAgainClicked;
    bool gameClosed;
    Vector achievedGameScore{};

  public:
    explicit MenuEngine(ResourceManager &resourceManager);
    std::map<ScreenState, std::shared_ptr<Screen>> screens;
    ScreenState getScreenState();
    /**
     * This method changes the screen state to the given screen state.
     * @param screenState
     */
    void switchTo(ScreenState screenState);
    /**
     * This method changes the currently selected button on the currently displayed screen.
     * The next button which is selected is the next button in the list.
     * @param offset
     */
    void switchButton(int offset);
    /**
     * This method changes the current screen state based on the currently selected button.
     */
    void switchScreen();
    void setGameplayRunning(bool isGameplayRunning);
    bool isGamePlayRunning() const;
    bool getPlayAgainClicked() const;
    void setPlayAgainClicked(bool playAgainClicked);
    std::shared_ptr<Button> getCurrentlySelectedButton();
    bool isGameClosed() const;
    void setAchievedGameScore(const Vector &gameScore);
    Vector getAchievedGameScore();
};

#endif // SURVIVING_SARNTAL_MENUENGINE_H
