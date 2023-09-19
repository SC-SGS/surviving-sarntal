#include "render_systems.h"
#include "flecs.h"
#include "raylib.h"
#include <iostream>

void window_prepare_drawing_system(flecs::iter) { ClearBackground(WHITE); }

const int screenWidth = 800;
const int screenHeight = 450;

void toggle_fullscreen_system(flecs::iter) {
    
}

void update_window(flecs::iter) {

}

RenderSystems::RenderSystems(flecs::world &world) {
    world.module<RenderSystems>();

    world.system().kind(flecs::PreUpdate).iter(window_prepare_drawing_system);
    world.system().kind(flecs::PreUpdate).iter(toggle_fullscreen_system);
    world.system().kind(flecs::OnUpdate).iter(update_window);
}


Texture2D gradientTex;
void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradientTex); //TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradientTex = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

void main_loop(flecs::world &world) {
    InitWindow(screenWidth, screenHeight, WINDOW_NAME);
    
    regenerateGradientTexture(screenWidth, screenHeight);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        world.progress(dt);



        if (IsKeyPressed(KEY_F11)) {
            int display = GetCurrentMonitor();
            if (IsWindowFullscreen()) {
                // if we are full screen, then go back to the windowed size
                SetWindowSize(screenWidth, screenHeight);
                regenerateGradientTexture(screenWidth, screenHeight);

            } else {
                // if we are not full screen, set the window size to match the
                // monitor we are on
                SetWindowSize(GetMonitorWidth(display),
                              GetMonitorHeight(display));

                regenerateGradientTexture(GetMonitorWidth(display),
                                          GetMonitorHeight(display));
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
        EndDrawing();
    
    }
    CloseWindow();

    UnloadTexture(gradientTex);
}
