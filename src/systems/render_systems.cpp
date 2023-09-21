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
HANDLE spriteTex;

int rotation = 0;

void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradientTex); // TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradientTex = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

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

            flecs::filter<Position, SpriteComponent> q =
                world.filter<Position, SpriteComponent>();

            q.each([](Position &p, SpriteComponent &s) {
                if (s.resourceHandle != NULL_HANDLE) {
                    auto texture = graphics::res.textures.Get(s.resourceHandle);

                    Rectangle sourceRec = {
                        0.0f, 0.0f, (float)texture.width,
                        (float)texture.height}; // part of the texture used

                    Rectangle destRec = {p.x, p.y, s.width,
                                         s.height}; // where to draw texture

                    DrawTexturePro(
                        texture, sourceRec, destRec,
                        {(float)texture.width, (float)texture.height}, 0,
                        WHITE);
                }
            });

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
        c.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
        c.rotation = 0.0f;
        c.zoom = 1.0f;
    });

    // add the render system
    world.system().kind(flecs::PostUpdate).iter(render_system);

    // add the resource manager
    world.set<Resources>({});

    // misc
    regenerateGradientTexture(screenWidth, screenHeight);
    Image verticalGradient =
        GenImageGradientV(screenWidth / 5, screenHeight / 5, RED, YELLOW);
    // spriteTex = LoadTextureFromImage(verticalGradient);

    // add the camera entity here for now
    auto test_e = world.entity("TestEntity")
                      .set([&](SpriteComponent &c) {
                          c = {0};
                          c.resourceHandle = graphics::res.textures.Load(
                              LoadTextureFromImage(verticalGradient));
                          c.width = 100;
                          c.height = 100;
                      })
                      .set(([&](Position &c) {
                          c.x = 0;
                          c.y = 0;
                      }));
}

void destroy() {
    CloseWindow();
    UnloadTexture(gradientTex);
}

} // namespace graphics
