//
// Created by six on 5/4/24.
//
#include "StartScreen.h"
#include "raylib.h"

void StartScreen::draw() {
    BeginDrawing();
    {
        ClearBackground(BLACK);
        DrawText("Surviving Sarntal", 500, 350, 70, WHITE);
        DrawText("Press Enter to start", 450, 500, 70, WHITE);
    }
    EndDrawing();
}