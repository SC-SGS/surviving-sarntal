//
// Created by six on 5/4/24.
//
#include "StartScreen.h"
#include "../../utilities/GameConstants.hpp"
#include "raylib.h"

StartScreen::StartScreen(ResourceManager &resourceManager) : Screen(resourceManager) { this->setUp(); }

void StartScreen::setUp() {
    this->setUpScreen();
    this->setUpButtons();
}

void StartScreen::setUpScreen() {
    this->background = resourceManager.getTexture("startScreen");
    this->selectedButton = 0;
}

void StartScreen::setUpButtons() {
    buttons = std::vector<std::shared_ptr<Button>>();
    Texture2D startButtonTexture = resourceManager.getTexture("startButton");
    Texture2D startButtonToggledTexture = resourceManager.getTexture("startButtonToggled");
    std::shared_ptr<Button> startButton =
        std::make_shared<Button>(START_GAME, NO_MENU_SCREEN, startButtonTexture, startButtonToggledTexture);

    Texture settingsButtonTexture = resourceManager.getTexture("settingsButton");
    Texture settingsButtonToggledTexture = resourceManager.getTexture("settingsButtonToggled");
    std::shared_ptr<Button> settingsButton =
        std::make_shared<Button>(SETTINGS, SETTINGS_SCREEN, settingsButtonTexture, settingsButtonToggledTexture);

    Texture2D instructionsButtonTexture = resourceManager.getTexture("instructionsButton");
    Texture2D instructionsButtonToggledTexture = resourceManager.getTexture("instructionsButtonToggled");
    std::shared_ptr<Button> instructionsButton = std::make_shared<Button>(
        INSTRUCTIONS, INSTRUCTIONS_SCREEN, instructionsButtonTexture, instructionsButtonToggledTexture);
    startButton->toggleButton();

    buttons.push_back(startButton);
    buttons.push_back(instructionsButton);
    buttons.push_back(settingsButton);
    this->distributeButtons();
    this->setUpEndGameButton();
}

void StartScreen::setUpEndGameButton() {
    Texture2D closeGameButtonTexture = resourceManager.getTexture("closeGameButton");
    Texture2D closeGameButtonToggledTexture = resourceManager.getTexture("closeGameButtonToggled");
    std::shared_ptr<Button> closeGameButton =
        std::make_shared<Button>(CLOSE_GAME, NO_SCREEN, closeGameButtonTexture, closeGameButtonToggledTexture);
    closeGameButton->setHeight(90);
    closeGameButton->setWidth(90);
    Vector position = {static_cast<float>(GetScreenWidth()) - 50 - closeGameButton->getWidth(), 50};
    closeGameButton->setPosition(position);
    buttons.push_back(closeGameButton);
}

void StartScreen::distributeButtons() {
    int margin = 400;
    int usableWidth = GetScreenWidth() - 2 * margin;
    int totalButtonWidth = 0;
    int buttonCount = (int)buttons.size();
    for (const auto &button : buttons) {
        totalButtonWidth += (int)button->getWidth();
    }
    int totalSpacing = usableWidth - totalButtonWidth;
    int spacing = totalSpacing / (buttonCount - 1);
    int currentX = margin;
    for (auto &button : buttons) {
        Vector position = {0, static_cast<float>(GetScreenHeight()) - 200};
        position.x = (floatType)currentX;
        button->setPosition(position);
        currentX += (int)button->getWidth() + spacing;
    }
}
