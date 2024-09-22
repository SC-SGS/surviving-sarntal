//
// Created by Anietta Weckauff on 26.07.24.
//

#ifndef SURVIVING_SARNTAL_BUTTON_H
#define SURVIVING_SARNTAL_BUTTON_H

#include "../../utilities/vector.h"
#include "../MenuEnums.h"
#include "raylib.h"
#include <string>

class Button {
  private:
    ButtonType buttonType;
    ScreenState resultingScreen;
    Texture2D texture{};
    bool isToggled;
    Texture2D untoggledButton;
    Texture2D toggledButton;
    Vector position{};
    floatType width;
    floatType height;

  public:
    Button(ButtonType buttonType, ScreenState resultingScreen, Texture2D untoggledButton, Texture2D toggledButton);
    Vector getPosition();
    void setPosition(Vector &position);
    void toggleButton();
    floatType getWidth() const;
    void setWidth(floatType width);
    floatType getHeight() const;
    void setHeight(floatType height);
    Texture2D getTexture();
    ScreenState getResultingScreen();
    ButtonType getButtonType();
};

#endif // SURVIVING_SARNTAL_BUTTON_H
