//
// Created by Anietta Weckauff on 04.08.24.
//

#ifndef SURVIVING_SARNTAL_PAUSESCREEN_H
#define SURVIVING_SARNTAL_PAUSESCREEN_H

#include "Screen.h"

class PauseScreen : public Screen {
  public:
    explicit PauseScreen(ResourceManager &resourceManager);

  private:
    void setUpScreen() override;
    void setUpButtons() override;
    void distributeButtons();
    void setUp();
};

#endif // SURVIVING_SARNTAL_PAUSESCREEN_H
