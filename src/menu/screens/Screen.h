//
// Created by Anietta Weckauff on 26.07.24.
//

#ifndef SURVIVING_SARNTAL_SCREEN_H
#define SURVIVING_SARNTAL_SCREEN_H

#include "../../output/ResourceManager.h"
#include "../menuElements/Button.h"
#include "raylib.h"
#include <list>

class Screen {

  public:
    virtual void setUpScreen();
    virtual void setUpButtons();

  protected:
    ResourceManager &resourceManager;
    Texture2D background{};
    std::vector<std::shared_ptr<Button>> buttons;
    size_t selectedButton;

  public:
    explicit Screen(ResourceManager &resourceManager);
    void switchButton(int offset);
    Texture2D getBackground();
    std::vector<std::shared_ptr<Button>> getButtons();
    std::shared_ptr<Button> getCurrentlySelectedButton();
};

#endif // SURVIVING_SARNTAL_SCREEN_H
