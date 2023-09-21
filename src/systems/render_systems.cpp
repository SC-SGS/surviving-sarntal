#include "render_systems.h"
#include "flecs.h"
#include "raygui.h"
#include "raylib.h"
#include <iostream>
#define RAYGUI_IMPLEMENTATION

void window_prepare_drawing_system(flecs::iter) { ClearBackground(WHITE); }

void toggle_fullscreen_system(flecs::iter) {
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }
}

void update_window(flecs::iter) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);
    EndDrawing();
}

RenderSystems::RenderSystems(flecs::world &world) {
    world.module<RenderSystems>();

    world.system().kind(flecs::PreUpdate).iter(window_prepare_drawing_system);
    world.system().kind(flecs::PreUpdate).iter(toggle_fullscreen_system);
    world.system().kind(flecs::OnUpdate).iter(update_window);
}

void main_loop(flecs::world &world) {
    InitWindow(100, 100, WINDOW_NAME);
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        world.progress(dt);
    }
    CloseWindow();
}
