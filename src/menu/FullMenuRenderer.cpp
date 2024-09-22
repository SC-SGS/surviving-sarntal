//
// Created by Anietta Weckauff on 04.08.24.
//

#include "FullMenuRenderer.h"

FullMenuRenderer::FullMenuRenderer(MenuEngine &menuEngine) : menuEngine(menuEngine) {}

void FullMenuRenderer::render() {
    ScreenState screenState = menuEngine.getScreenState();

    if (screenState != NO_MENU_SCREEN) {
        BeginDrawing();
        this->renderBackground(screenState);
        this->renderButtons(screenState);
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
    const Rectangle sourceRec = {0.0f, 0.0f, graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL};
    const Rectangle destRec = {0.0f, 0.0f, graphics::SCREEN_WIDTH_IN_PIXEL, graphics::SCREEN_HEIGHT_IN_PIXEL};

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
