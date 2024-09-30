//
// Created by Anietta Weckauff on 26.07.24.
//

#include "MenuEngine.h"

MenuEngine::MenuEngine(ResourceManager &resourceManager) : resourceManager(resourceManager) {
    std::cout << "constructor menu engine" << std::endl;
    screens[START_SCREEN] = std::make_shared<StartScreen>(resourceManager);
    screens[SETTINGS_SCREEN] = std::make_shared<SettingsScreen>(resourceManager);
    screens[INSTRUCTIONS_SCREEN] = std::make_shared<InstructionsScreen>(resourceManager);
    screens[END_SCREEN] = std::make_shared<EndScreen>(resourceManager);
    screens[PAUSE_SCREEN] = std::make_shared<PauseScreen>(resourceManager);
    // screens[NO_MENU_SCREEN] = std::make_shared<StartScreen>(resourceManager);

    currentScreen = START_SCREEN;
    gameplayRunning = false;
    playAgainClicked = false;
    gameClosed = false;
    achievedGameScore = {0, 0};
}
ScreenState MenuEngine::getScreenState() { return currentScreen; }

void MenuEngine::switchButton(int offset) { screens[currentScreen]->switchButton(offset); }

void MenuEngine::switchTo(ScreenState screenState) { this->currentScreen = screenState; }
void MenuEngine::switchScreen() {
    ButtonType currentButton = screens[currentScreen]->getCurrentlySelectedButton()->getButtonType();
    if (currentButton == CLOSE_GAME) {
        this->gameClosed = true;
    }
    ScreenState nextScreenState = screens[currentScreen]->getCurrentlySelectedButton()->getResultingScreen();
    this->switchTo(nextScreenState);
    if (nextScreenState == NO_MENU_SCREEN) {
        this->gameplayRunning = true;
    }
    if (currentButton == PLAY_AGAIN || currentButton == BACK_TO_MENU) {
        this->playAgainClicked = true;
    }
}
void MenuEngine::setGameplayRunning(bool isGameplayRunning) { this->gameplayRunning = isGameplayRunning; }

bool MenuEngine::isGamePlayRunning() const { return gameplayRunning; }

std::shared_ptr<Button> MenuEngine::getCurrentlySelectedButton() {
    return screens[currentScreen]->getCurrentlySelectedButton();
}

bool MenuEngine::getPlayAgainClicked() const { return playAgainClicked; }

void MenuEngine::setPlayAgainClicked(bool playAgain) { this->playAgainClicked = playAgain; }

bool MenuEngine::isGameClosed() const { return this->gameClosed; }

void MenuEngine::setAchievedGameScore(const Vector &gameScore) { this->achievedGameScore = gameScore; }

Vector MenuEngine::getAchievedGameScore() { return this->achievedGameScore; }

void MenuEngine::resetScreens() {
    for (auto &screen : screens) {
        screen.second->setUpButtons();
    }
}