//
// Created by Anietta Weckauff on 04.08.24.
//

#include "FullMenuRenderer.h"
#include <iomanip>
#include <iostream>
#include <sstream>

FullMenuRenderer::FullMenuRenderer(MenuEngine &menuEngine) : menuEngine(menuEngine) {}

void FullMenuRenderer::render() {
    ScreenState screenState = menuEngine.getScreenState();

    if (screenState != NO_MENU_SCREEN) {
        BeginDrawing();
        this->renderBackground(screenState);
        this->renderButtons(screenState);
        if (screenState == END_SCREEN) {
            this->renderScoreOnEndscreen();
        }
        EndDrawing();
    }
}

void FullMenuRenderer::renderButtons(ScreenState screenState) {
    std::vector<std::shared_ptr<Button>> buttons = menuEngine.screens[screenState]->getButtons();
    for (const std::shared_ptr<Button> &button : buttons) {
        this->renderButton(button);
    }
}

void FullMenuRenderer::renderBackground(ScreenState screenState) {
    Texture2D backgroundTexture = menuEngine.screens[screenState]->getBackground();
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(backgroundTexture.width),
                                 static_cast<float>(backgroundTexture.height)};
    const Rectangle destRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    DrawTexturePro(backgroundTexture, sourceRec, destRec, {0, 0}, 0, WHITE);
}

void FullMenuRenderer::renderButton(const std::shared_ptr<Button> &button) {
    const Texture2D texture = button->getTexture();
    const floatType width = button->getWidth();
    const floatType height = button->getHeight();
    const Vector position = button->getPosition();

    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<floatType>(texture.width),
                                 static_cast<floatType>(texture.height)};
    const Rectangle destRec = {position.x, position.y, width, height};
    // const Vector2 origin = {width / 2, height / 2};

    DrawTexturePro(texture, sourceRec, destRec, {0, 0}, 0, WHITE);
}

void FullMenuRenderer::renderScoreOnEndscreen() {
    Vector endScore = menuEngine.getAchievedGameScore();

    std::ostringstream outX;
    outX << std::fixed << std::setprecision(1) << std::round(endScore.x * 10) / 10;
    std::string xScoreString = outX.str() + "m";

    std::ostringstream outY;
    outY << std::fixed << std::setprecision(1) << std::round(endScore.y * 10) / 10;
    std::string yScoreString = outY.str() + "m";

    const char *xScoreText = xScoreString.c_str();
    const char *yScoreText = yScoreString.c_str();

    const auto xScoreXPos = GetScreenWidth() / 2 - 100;
    const auto xScoreYpos = 6 * GetScreenHeight() / 7 - 10;
    const auto yScoreXPos = GetScreenWidth() / 2 + 70;
    const auto yScoreYpos = 6 * GetScreenHeight() / 7 - 10;
    const auto fontSize = static_cast<int>(static_cast<float>(GetScreenWidth()) * 0.0125f);
    DrawText(xScoreText, xScoreXPos, xScoreYpos, fontSize, WHITE);
    DrawText(yScoreText, yScoreXPos, yScoreYpos, fontSize, WHITE);
}
