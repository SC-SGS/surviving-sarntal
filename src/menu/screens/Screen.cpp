//
// Created by Anietta Weckauff on 26.07.24.
//
#include "Screen.h"

// I know that it doesn't make sense that startScreen is a default, but clang tidy
Screen::Screen(ResourceManager &resourceManager)
    : resourceManager(resourceManager), selectedButton(0), background(resourceManager.getTexture("startScreen")) {}

void Screen::switchButton(int offset) {
    buttons[selectedButton]->toggleButton();
    selectedButton = (selectedButton + offset) % buttons.size();
    buttons[selectedButton]->toggleButton();
}

Texture2D Screen::getBackground() { return background; }
std::vector<std::shared_ptr<Button>> Screen::getButtons() { return buttons; }
void Screen::setUpScreen() {}
void Screen::setUpButtons() {}
std::shared_ptr<Button> Screen::getCurrentlySelectedButton() { return buttons[selectedButton]; }
