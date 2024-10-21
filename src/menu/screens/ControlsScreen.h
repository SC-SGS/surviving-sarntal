//
// Created by Anietta Weckauff on 04.08.24.
//

#ifndef SURVIVING_SARNTAL_CONTROLSSCREEN_H
#define SURVIVING_SARNTAL_CONTROLSSCREEN_H

#include "../../output/ResourceManager.h"
#include "Screen.h"

class ControlsScreen : public Screen {

  public:
    explicit ControlsScreen(ResourceManager &resourceManager);

  private:
    void setUpScreen() override;
    void setUpButtons() override;
    void setUp();
};

#endif // SURVIVING_SARNTAL_CONTROLSSCREEN_H
