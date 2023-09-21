#include "render_systems.h"
#include "../components/particle_state.h"
#include "../components/render_components.h"
#include "flecs.h"
#include "raylib.h"
#include <iostream>

namespace graphics {

const int screenWidth = 800;
const int screenHeight = 450;

Texture2D gradientTex;
void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradientTex); // TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradientTex = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

void render_sytem(flecs::iter iter) {
    auto &world = iter.world();

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
    if (WindowShouldClose()) {
        // switch Appinfo isRunning to false;
        auto info = world.get<AppInfo>();
        AppInfo info_ = *info;
        info_.isRunning = false;
        world.set<AppInfo>(info_);
    }

    BeginDrawing();
    ClearBackground(BLUE);

    DrawTexture(gradientTex, 0, 0, WHITE);

    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);

    auto camera = world.lookup("Camera");

    // BeginMode2D(camera);
    //// DrawLine(-100, -100, 100, 100, GREEN);
    // DrawLine(-screenWidth * 10, screenHeight * 10, screenWidth * 10,
    //          -screenHeight * 10, GREEN);
    ////DrawRectangleRec(player, RED);
    // EndMode2D();

    // loop for all sprites (sprite component + transform compoenent)

    // loor for all

    EndDrawing();
}

void init_render_sytem(flecs::world &world) {
    // std::cout << "-------init render" << std::endl;
    InitWindow(screenWidth, screenHeight, WINDOW_NAME);

    auto camera = world.entity("Camera").set([](Camera2DComponent &c) {
        c = {0};
        c.target = {0.0f, 0.0f};
        c.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
        c.rotation = 0.0f;
        c.zoom = 1.0f;
    });

    auto cara = world.lookup("Camera");
    regenerateGradientTexture(screenWidth, screenHeight);
    world.system().kind(flecs::PostUpdate).iter(render_sytem);
}

void destroy() {
    CloseWindow();
    UnloadTexture(gradientTex);
}

} // namespace graphics
