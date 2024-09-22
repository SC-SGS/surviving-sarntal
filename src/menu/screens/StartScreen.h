//
// Created by six on 5/4/24.
//

#ifndef SURVIVING_SARNTAL_STARTSCREEN_H
#define SURVIVING_SARNTAL_STARTSCREEN_H

#include "Screen.h"

class StartScreen : public Screen {

  public:
    explicit StartScreen(ResourceManager &resourceManager);

  private:
    void setUpScreen() override;
    void setUpButtons() override;
    void distributeButtons();
    void setUp();
    void setUpEndGameButton();
};

#endif // SURVIVING_SARNTAL_STARTSCREEN_H
