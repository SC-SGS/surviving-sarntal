#include "render_systems.h"
#include "../components/render_components.h"
#include "flecs.h"
#include "raylib.h"
#include <iostream>

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

void render_sytem(flecs::iter iter) {
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
    // DrawLine(-100, -100, 100, 100, GREEN);

    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);

    auto world = iter.world();
    /* auto cameras = world.filter_builder<Camera2DComponent>().build();
    auto camera = cameras.first();*/

    auto camera = iter.world().lookup("Camera");

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

RenderSystems::RenderSystems(flecs::world &world) {
    world.module<RenderSystems>();

    // world.system().kind(flecs::PreUpdate).iter(window_prepare_drawing_system);
    // world.system().kind(flecs::PreUpdate).iter(toggle_fullscreen_system);
    world.system().kind(flecs::PostUpdate).iter(render_sytem);

    init(world);
}

void RenderSystems::init(flecs::world &world) {
    InitWindow(screenWidth, screenHeight, WINDOW_NAME);
    /*
    auto camera = world.entity().set([](Camera2DComponent &c) {
        c = {0};
        c.target = {0.0f, 0.0f};
        c.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
        c.rotation = 0.0f;
        c.zoom = 1.0f;
        });*/
    auto camera = world.entity("Camera");
    auto c = world.lookup("Camera");

    /*Camera2DComponent c = {0};
    c.target = {0.0, 0.0,};
    c.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
    c.rotation = 0.0f;
    c.zoom = 1.0f;

    camera.set(c);*/

    regenerateGradientTexture(screenWidth, screenHeight);
}

void RenderSystems::destroy() {
    CloseWindow();
    UnloadTexture(gradientTex);
}

RenderSystems::~RenderSystems() { destroy(); }

} // namespace graphics
