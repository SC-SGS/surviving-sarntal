//
// Created by Anietta Weckauff on 04.08.24.
//

#ifndef SURVIVING_SARNTAL_SETTINGSSCREEN_H
#define SURVIVING_SARNTAL_SETTINGSSCREEN_H

#include "../../output/ResourceManager.h"
#include "Screen.h"

class SettingsScreen : public Screen {

  public:
    explicit SettingsScreen(ResourceManager &resourceManager);

  private:
    void setUpScreen() override;
    void setUpButtons() override;
    void setUp();
};

#endif // SURVIVING_SARNTAL_SETTINGSSCREEN_H
