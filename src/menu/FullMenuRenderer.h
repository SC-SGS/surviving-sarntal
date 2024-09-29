//
// Created by Anietta Weckauff on 04.08.24.
//

#ifndef SURVIVING_SARNTAL_FULLMENURENDERER_H
#define SURVIVING_SARNTAL_FULLMENURENDERER_H

#include "MenuEngine.h"

class FullMenuRenderer {

  private:
    MenuEngine &menuEngine;
    void renderButtons(ScreenState screenState);
    void renderBackground(ScreenState screenState);
    void renderButton(const std::shared_ptr<Button> &button);
    void renderScoreOnEndscreen();

  public:
    void render();
    explicit FullMenuRenderer(MenuEngine &menuEngine);
};

#endif // SURVIVING_SARNTAL_FULLMENURENDERER_H
