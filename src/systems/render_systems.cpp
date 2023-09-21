#include "render_systems.h"
#include "../components/particle_state.h"
#include "../components/render_components.h"
// #include "../utils/resource_manager.h"
#include "flecs.h"
#include <iostream>

namespace graphics {

const int screenWidth = 800;
const int screenHeight = 450;

Texture2D gradientTex;
Texture2D spriteTex;
Rectangle sourceRec;
Rectangle destRec;
int rotation = 0;

void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradientTex); // TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradientTex = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

Vector2 points[] = {
    {50, 190},  {100, 110}, {150, 200}, {200, 100},
    {250, 130}, {300, 210}, {350, 90},  {400, 150},
};

Vector2 generateControlPoints(Vector2 points) { Vector2 control_points; };

void render_system(flecs::iter &iter) {
    auto world = iter.world();

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
        auto info = world.get_mut<AppInfo>();
        info->isRunning = false;
    }

    auto camera_entity = world.lookup("Camera");
    if (camera_entity.is_valid()) {
        auto camera = camera_entity.get<Camera2DComponent>();

        BeginDrawing();
        ClearBackground(BLUE);

        BeginMode2D(*camera);
        {
            DrawTexture(gradientTex, 0, 0, WHITE);

            // DrawText("Congrats! You created your first window!", 190, 200,
            // 20, LIGHTGRAY);

            // loop for all sprites (sprite component + transform compoenent)

            // loor for all

            for (int i = 0; i < sizeof(points) / sizeof(points[0]) - 1; i++) {
                DrawLineBezierCubic(points[i], points[i + 1], points[i],
                                    points[i + 1], 5, RED);
            }

            // Draw the control points and lines
            if (DEBUG) {
                for (int i = 0; i < sizeof(points) / sizeof(points[0]); i++) {
                    DrawCircleV(points[i], 5,
                                BLUE); // Draw control points as circles
                }
            }

            DrawTexturePro(spriteTex, sourceRec, destRec,
                           {(float)spriteTex.width, (float)spriteTex.height},
                           rotation, WHITE);
            rotation++;
        }

        EndMode2D();

        EndDrawing();
    }
}

void init_render_system(flecs::world &world) {
    InitWindow(screenWidth, screenHeight, WINDOW_NAME);

    // add the camera entity here for now
    auto camera = world.entity("Camera").set([](Camera2DComponent &c) {
        c = {0};
        c.target = {0.0f, 0.0f};
        c.offset = {0.0f, screenHeight / 2.0f};
        c.rotation = 0.0f;
        c.zoom = 1.0f;
    });

    // add the render system
    world.system().kind(flecs::PostUpdate).iter(render_system);

    // add the resource manager
    // world.set<Resources>({});

    // misc
    regenerateGradientTexture(screenWidth, screenHeight);
    Image verticalGradient =
        GenImageGradientV(screenWidth / 5, screenHeight / 5, RED, YELLOW);
    spriteTex = LoadTextureFromImage(verticalGradient);
    sourceRec = {0.0f, 0.0f, (float)spriteTex.width,
                 (float)spriteTex.height}; // part of the texture used
    destRec = {screenWidth / 2.0f, screenHeight / 2.0f, spriteTex.width * 2.0f,
               spriteTex.height * 2.0f}; // where to draw texture
}

void destroy() {
    CloseWindow();
    UnloadTexture(gradientTex);
}

} // namespace graphics
