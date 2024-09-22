//
// Created by Anietta Weckauff on 04.08.24.
//

#ifndef SURVIVING_SARNTAL_ENDSCREEN_H
#define SURVIVING_SARNTAL_ENDSCREEN_H

#include "Screen.h"
class EndScreen : public Screen {
  public:
    explicit EndScreen(ResourceManager &resourceManager);

  private:
    void setUpScreen() override;
    void setUpButtons() override;
    void setUp();
};

#endif // SURVIVING_SARNTAL_ENDSCREEN_H
