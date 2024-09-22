//
// Created by Anietta Weckauff on 04.08.24.
//

#include "EndScreen.h"

EndScreen::EndScreen(ResourceManager &resourceManager) : Screen(resourceManager) { this->setUp(); }

void EndScreen::setUp() {
    this->setUpScreen();
    this->setUpButtons();
}

void EndScreen::setUpScreen() {
    this->background = resourceManager.getTexture("endScreen");
    this->selectedButton = 0;
}

void EndScreen::setUpButtons() {
    Texture2D playAgainButtonTexture = resourceManager.getTexture("playAgainButton");
    Texture2D playAgainButtonToggledTexture = resourceManager.getTexture("playAgainButtonToggled");
    std::shared_ptr<Button> playAgainButton =
        std::make_shared<Button>(PLAY_AGAIN, START_SCREEN, playAgainButtonTexture, playAgainButtonToggledTexture);
    Vector position = {(floatType)graphics::SCREEN_WIDTH_IN_PIXEL / 2 - playAgainButton->getWidth() / 2,
                       (floatType)graphics::SCREEN_HEIGHT_IN_PIXEL / 2 + playAgainButton->getHeight() / 2};
    playAgainButton->setPosition(position);
    playAgainButton->toggleButton();
    buttons.push_back(playAgainButton);

    Texture2D closeGameButtonTexture = resourceManager.getTexture("closeGameButton");
    Texture2D closeGameButtonToggledTexture = resourceManager.getTexture("closeGameButtonToggled");
    std::shared_ptr<Button> closeGameButton =
        std::make_shared<Button>(CLOSE_GAME, NO_SCREEN, closeGameButtonTexture, closeGameButtonToggledTexture);

    closeGameButton->setHeight(90);
    closeGameButton->setWidth(90);
    Vector position2 = {graphics::SCREEN_WIDTH_IN_PIXEL - 50 - closeGameButton->getWidth(), 50};
    closeGameButton->setPosition(position2);
    buttons.push_back(closeGameButton);
}
