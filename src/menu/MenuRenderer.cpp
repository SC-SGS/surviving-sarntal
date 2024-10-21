//
// Created by Anietta Weckauff on 11.07.24.
//

#include "MenuRenderer.h"

MenuRenderer::MenuRenderer(ResourceManager &resourceManager) : resourceManager(resourceManager) {}

void MenuRenderer::drawStartScreen() {
    checkMouseClickStartScreen();

    const Texture2D texture = resourceManager.getTexture("startScreen");
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()),
                                 static_cast<float>(GetScreenHeight())};
    const Rectangle destRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    BeginDrawing();
    DrawTexturePro(texture, sourceRec, destRec, {0, 0}, 0, WHITE);
    EndDrawing();
}

void MenuRenderer::checkMouseClickStartScreen() {
    Rectangle buttonBoundsStartGame = {412.6, 694.7, 227.4, 118.2};
    Rectangle buttonBoundsInstructions = {686.3, 694.7, 227.4, 118.2};
    Rectangle buttonBoundsSettings = {959.7, 694.7, 227.4, 118.2};

    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, buttonBoundsStartGame)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            this->gameplayRunning = true;
        }
    }

    if (CheckCollisionPointRec(mousePoint, buttonBoundsInstructions)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            this->setScreenState(INSTRUCTIONS_SCREEN);
        }
    }

    if (CheckCollisionPointRec(mousePoint, buttonBoundsSettings)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            this->setScreenState(CONTROLS_SCREEN);
        }
    }
}

void MenuRenderer::drawEndScreen() {
    const Texture2D texture = resourceManager.getTexture("endScreen");
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()),
                                 static_cast<float>(GetScreenHeight())};
    const Rectangle destRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    BeginDrawing();
    DrawTexturePro(texture, sourceRec, destRec, {0, 0}, 0, WHITE);
    EndDrawing();

    checkMouseClickEndScreen();
}

void MenuRenderer::checkMouseClickEndScreen() {
    Rectangle buttonBoundsHome = {686.3, 540.5, 227.4, 118.2};

    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, buttonBoundsHome)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            this->setScreenState(START_SCREEN);
            this->playAgainClicked = true;
        }
    }
}

void MenuRenderer::drawControlsScreen() {
    const Texture2D texture = resourceManager.getTexture("settingsScreen");
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()),
                                 static_cast<float>(GetScreenHeight())};
    const Rectangle destRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    BeginDrawing();
    DrawTexturePro(texture, sourceRec, destRec, {0, 0}, 0, WHITE);
    EndDrawing();

    checkMouseClickHomeButton();
}

void MenuRenderer::checkMouseClickHomeButton() {
    Rectangle buttonBoundsHome = {1460, 49.9, 100, 100};

    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, buttonBoundsHome)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            this->setScreenState(START_SCREEN);
        }
    }
}

void MenuRenderer::drawInstructionsScreen() {
    const Texture2D texture = resourceManager.getTexture("instructionScreen");
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()),
                                 static_cast<float>(GetScreenHeight())};
    const Rectangle destRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    BeginDrawing();
    DrawTexturePro(texture, sourceRec, destRec, {0, 0}, 0, WHITE);
    EndDrawing();

    checkMouseClickHomeButton();
}

void MenuRenderer::draw() {
    switch (this->screenState) {
    case START_SCREEN:
        drawStartScreen();
        break;
    case INSTRUCTIONS_SCREEN:
        drawInstructionsScreen();
        break;
    case CONTROLS_SCREEN:
        drawControlsScreen();
        break;
    case END_SCREEN:
        drawEndScreen();
        break;
    case PAUSE_SCREEN:
        drawPauseScreen();
        break;
    case NO_MENU_SCREEN:
        break;
    }
}
void MenuRenderer::setScreenState(ScreenState newScreenState) { this->screenState = newScreenState; }

ScreenState MenuRenderer::getScreenState() { return CONTROLS_SCREEN; }

bool MenuRenderer::isGameplayRunning() const { return this->gameplayRunning; }
void MenuRenderer::setGameplayRunning(bool newGameplayRunning) { this->gameplayRunning = newGameplayRunning; }
bool MenuRenderer::isPlayAgainClicked() const { return this->playAgainClicked; }
void MenuRenderer::setPlayAgainClicked(bool newPlayAgainClicked) { this->playAgainClicked = newPlayAgainClicked; }

void MenuRenderer::drawPauseScreen() {
    const Texture2D texture = resourceManager.getTexture("pauseScreen");
    const Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()),
                                 static_cast<float>(GetScreenHeight())};
    const Rectangle destRec = {0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

    BeginDrawing();
    DrawTexturePro(texture, sourceRec, destRec, {0, 0}, 0, WHITE);
    EndDrawing();
}
