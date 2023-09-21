#include "render_systems.h"
#include "flecs.h"
#include "raygui.h"
#include "raylib.h"
#include <iostream>
#define RAYGUI_IMPLEMENTATION

namespace graphics {

// void window_prepare_drawing_system(flecs::iter) { ClearBackground(WHITE); }

const int screenWidth = 800;
const int screenHeight = 450;

// void toggle_fullscreen_system(flecs::iter) {
//
// }

Texture2D gradientTex;
void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradientTex); // TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradientTex = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

void render_sytem(flecs::iter) {
    if (IsKeyPressed(KEY_F11)) {
        int display = GetCurrentMonitor();
        if (!IsWindowFullscreen()) {
            // if we are not full screen, set the window size to match the
            // monitor we are on
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));

            regenerateGradientTexture(GetMonitorWidth(display),
                                      GetMonitorHeight(display));

        } else {
            // if we are full screen, then go back to the windowed size
            SetWindowSize(screenWidth, screenHeight);
            regenerateGradientTexture(screenWidth, screenHeight);
        }

        // toggle the state
        ToggleFullscreen();
    }

    BeginDrawing();
    ClearBackground(BLUE);

    DrawTexture(gradientTex, 0, 0, WHITE);
    DrawLine(-100, -100, 100, 100, GREEN);

    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);

    // loop for all sprites (sprite component + transform compoenent)

    // loor for all

    EndDrawing();
}

RenderSystems::RenderSystems(flecs::world &world) {
    world.module<RenderSystems>();

    // world.system().kind(flecs::PreUpdate).iter(window_prepare_drawing_system);
    // world.system().kind(flecs::PreUpdate).iter(toggle_fullscreen_system);
    world.system().kind(flecs::PostUpdate).iter(render_sytem);

    init();
}

void RenderSystems::init() {
    InitWindow(screenWidth, screenHeight, WINDOW_NAME);

    regenerateGradientTexture(screenWidth, screenHeight);
}

void RenderSystems::destroy() {
    CloseWindow();
    UnloadTexture(gradientTex);
}

RenderSystems::~RenderSystems() { destroy(); }

} // namespace graphics
