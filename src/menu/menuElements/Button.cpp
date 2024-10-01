//
// Created by Anietta Weckauff on 26.07.24.
//
#include "Button.h"

Button::Button(ButtonType buttonType, ScreenState resultingScreen, Texture2D untoggledButton, Texture2D toggledButton)
    : buttonType(buttonType),
      resultingScreen(resultingScreen),
      untoggledButton(untoggledButton),
      toggledButton(toggledButton) {
    this->height = static_cast<float>(GetScreenWidth()) * 0.0625f;
    this->width = static_cast<float>(GetScreenWidth()) * 0.125f;
    this->texture = untoggledButton;
    this->isToggled = false;
    this->position = {0, 0};
}

void Button::toggleButton() {
    if (!isToggled) {
        isToggled = true;
        this->texture = toggledButton;
    } else {
        isToggled = false;
        this->texture = untoggledButton;
    }
}

floatType Button::getWidth() const { return this->width; }
floatType Button::getHeight() const { return this->height; }

void Button::setPosition(Vector &newPosition) { this->position = newPosition; }
Vector Button::getPosition() { return position; }
void Button::setWidth(floatType newWidth) { this->width = newWidth; }
void Button::setHeight(floatType newHeight) { this->height = newHeight; }
Texture2D Button::getTexture() { return texture; }
ScreenState Button::getResultingScreen() { return resultingScreen; }
ButtonType Button::getButtonType() { return buttonType; }
