//
// Created by Anietta Weckauff on 04.08.24.
//

#include "PauseScreen.h"

PauseScreen::PauseScreen(ResourceManager &resourceManager) : Screen(resourceManager) { this->setUp(); }

void PauseScreen::setUp() {
    this->setUpScreen();
    this->setUpButtons();
}

void PauseScreen::setUpScreen() {
    this->background = resourceManager.getTexture("pauseScreen");
    this->selectedButton = 0;
}

void PauseScreen::setUpButtons() {
    buttons = std::vector<std::shared_ptr<Button>>();
    Texture2D continueButtonTexture = resourceManager.getTexture("continueButton");
    Texture2D continueButtonToggledTexture = resourceManager.getTexture("continueButtonToggled");
    std::shared_ptr<Button> continueButton =
        std::make_shared<Button>(CONTINUE_GAME, NO_MENU_SCREEN, continueButtonTexture, continueButtonToggledTexture);

    Texture2D backToMenuButtonTexture = resourceManager.getTexture("backToMenuButton");
    Texture2D backToMenuToggledTexture = resourceManager.getTexture("backToMenuButtonToggled");
    std::shared_ptr<Button> backToMenuButton =
        std::make_shared<Button>(BACK_TO_MENU, START_SCREEN, backToMenuButtonTexture, backToMenuToggledTexture);

    Texture2D closeGameButtonTexture = resourceManager.getTexture("closeGameButton");
    Texture2D closeGameButtonToggledTexture = resourceManager.getTexture("closeGameButtonToggled");
    std::shared_ptr<Button> closeGameButton =
        std::make_shared<Button>(CLOSE_GAME, NO_SCREEN, closeGameButtonTexture, closeGameButtonToggledTexture);

    continueButton->toggleButton();
    buttons.push_back(continueButton);
    buttons.push_back(backToMenuButton);
    this->distributeButtons();

    closeGameButton->setHeight(90);
    closeGameButton->setWidth(90);
    Vector position = {static_cast<float>(GetScreenWidth()) - 50 - closeGameButton->getWidth(), 50};
    closeGameButton->setPosition(position);
    buttons.push_back(closeGameButton);
}

void PauseScreen::distributeButtons() {
    int margin = 500;                                // Minimum margin from the left and right edges
    int usableWidth = GetScreenWidth() - 2 * margin; // Usable width of the screen

    int totalButtonWidth = 0;
    int buttonCount = (int)buttons.size();

    for (const auto &button : buttons) {
        totalButtonWidth += (int)button->getWidth();
    }

    // Calculate the total space to be distributed between buttons
    int totalSpacing = usableWidth - totalButtonWidth;

    // Calculate the spacing between buttons
    int spacing = totalSpacing / (buttonCount - 1);

    // Calculate initial x-coordinate
    int currentX = margin;

    // Set x-coordinate for each button
    for (auto &button : buttons) {
        Vector position = {0, (floatType) static_cast<float>(GetScreenHeight()) / 2 + button->getHeight() / 2};
        position.x = (floatType)currentX;
        button->setPosition(position);
        currentX += (int)button->getWidth() + spacing;
    }
}
