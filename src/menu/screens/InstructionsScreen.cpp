//
// Created by Anietta Weckauff on 04.08.24.
//

#include "InstructionsScreen.h"

InstructionsScreen::InstructionsScreen(ResourceManager &resourceManager) : Screen(resourceManager) { this->setUp(); }

void InstructionsScreen::setUp() {
    this->setUpScreen();
    this->setUpButtons();
}

void InstructionsScreen::setUpScreen() {
    this->background = resourceManager.getTexture("instructionScreen");
    this->selectedButton = 0;
}

void InstructionsScreen::setUpButtons() {
    buttons = std::vector<std::shared_ptr<Button>>();
    Texture2D homeButtonTexture = resourceManager.getTexture("homeButton");
    Texture2D homeButtonToggledTexture = resourceManager.getTexture("homeButtonToggled");
    std::shared_ptr<Button> homeButton =
        std::make_shared<Button>(HOME, START_SCREEN, homeButtonTexture, homeButtonToggledTexture);
    homeButton->setHeight(90);
    homeButton->setWidth(90);
    Vector position = {static_cast<float>(GetScreenWidth()) - 100 - 2 * homeButton->getWidth(), 50};
    homeButton->setPosition(position);
    homeButton->toggleButton();
    buttons.push_back(homeButton);

    Texture2D closeGameButtonTexture = resourceManager.getTexture("closeGameButton");
    Texture2D closeGameButtonToggledTexture = resourceManager.getTexture("closeGameButtonToggled");
    std::shared_ptr<Button> closeGameButton =
        std::make_shared<Button>(CLOSE_GAME, NO_SCREEN, closeGameButtonTexture, closeGameButtonToggledTexture);

    closeGameButton->setHeight(90);
    closeGameButton->setWidth(90);
    Vector position2 = {static_cast<float>(GetScreenWidth()) - 50 - closeGameButton->getWidth(), 50};
    closeGameButton->setPosition(position2);
    buttons.push_back(closeGameButton);
}
