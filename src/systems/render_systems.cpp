#include "render_systems.h"
#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "../components/render_components.h"
#include "flecs.h"

namespace graphics {

Texture2D gradient_tex;

int rotation = 0;

bool use_debug_camera;
Camera2D debug_camera;

void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradient_tex); // TODO necessary?
    Image vertical_gradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradient_tex = LoadTextureFromImage(vertical_gradient);
    UnloadImage(vertical_gradient);
}

Vector2 points[] = {
    {50, 190},  {100, 110}, {150, 200}, {200, 100},
    {250, 130}, {300, 210}, {350, 90},  {400, 150},
};

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
            SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
            regenerateGradientTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // toggle the state
        ToggleFullscreen();
    }
    if (WindowShouldClose()) {
        // switch AppInfo isRunning to false;
        auto info = world.get_mut<AppInfo>();
        info->isRunning = false;
    }

    if (IsKeyPressed(KEY_P)) {
        use_debug_camera = !use_debug_camera;
    }

    auto camera_entity = world.lookup("Camera");
    if (camera_entity.is_valid()) {
        auto camera = camera_entity.get_mut<Camera2DComponent>();

        if (use_debug_camera) {
            if (IsKeyDown(KEY_D))
                debug_camera.target.x += 2;
            else if (IsKeyDown(KEY_A))
                debug_camera.target.x -= 2;
            else if (IsKeyDown(KEY_S))
                debug_camera.target.y += 2;
            else if (IsKeyDown(KEY_W))
                debug_camera.target.y -= 2;

            if (IsKeyDown(KEY_LEFT))
                debug_camera.rotation--;
            else if (IsKeyDown(KEY_RIGHT))
                debug_camera.rotation++;
        }

        auto interval =
            world.get_mut<Mountain>()->getIndexIntervalOfEntireMountain();

        BeginDrawing();
        {
            ClearBackground(BLUE);
            DrawTexture(gradient_tex, 0, 0, WHITE);

            if (use_debug_camera) {
                BeginMode2D(debug_camera);
            } else {
                BeginMode2D(*camera);
            }
            {
                // DrawText("Congrats! You created your first window!", 190,
                // 200, 20, LIGHTGRAY);

                // loop for all sprites (sprite component + transform
                // component)

                // loop for all
                auto mountain = world.get_mut<Mountain>();
                for (std::size_t i = interval.start_index;
                     i < interval.end_index; i++) {

                    Vector2 control_point_0{mountain->getVertex(i).x,
                                            -mountain->getVertex(i).y};
                    Vector2 control_point_1{mountain->getVertex(i + 1).x,
                                            -mountain->getVertex(i + 1).y};
                    Vector2 control_point_2{mountain->getVertex(i).x,
                                            -mountain->getVertex(i).y};
                    Vector2 control_point_3{mountain->getVertex(i + 1).x,
                                            -mountain->getVertex(i + 1).y};

                    DrawLineBezierCubic(control_point_0, control_point_1,
                                        control_point_2, control_point_3, 5,
                                        RED);
                }

                // Draw the control points and lines
                if (DEBUG) {
                    for (std::size_t i = interval.start_index;
                         i < interval.end_index; i++) {
                        Vector2 point = {mountain->getVertex(i).x,
                                         -mountain->getVertex(i).y};

                        DrawCircleV(point, 5,
                                    BLUE); // Draw control points as circles
                    }
                }

                flecs::filter<Position, SpriteComponent> q =
                    world.filter<Position, SpriteComponent>();

                q.each([&](Position &p, SpriteComponent &s) {
                    if (s.resourceHandle != NULL_HANDLE) {
                        auto texture = world.get_mut<Resources>()->textures.Get(
                            s.resourceHandle);

                        Rectangle source_rec = {
                            0.0f, 0.0f, (float)texture.width,
                            (float)texture.height}; // part of the texture used

                        Rectangle dest_rec = {
                            p.x, -p.y, static_cast<float>(s.width),
                            static_cast<float>(
                                s.height)}; // where to draw texture

                        DrawTexturePro(
                            texture, source_rec, dest_rec,
                            {(float)texture.width, (float)texture.height}, 0,
                            WHITE);
                    }
                });

                flecs::filter<Position, CircleShapeRenderComponent> circle_q =
                    world.filter<Position, CircleShapeRenderComponent>();

                circle_q.each([&](Position &p, CircleShapeRenderComponent &s) {
                    DrawCircle((int)p.x, (int)-p.y, s.radius, RED);
                });

                rotation++;
            }

            EndMode2D();
        }
        EndDrawing();
    }
}

void init_render_system(flecs::world &world) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME);

    // add the camera entity here for now
    world.entity("Camera").set([](Camera2DComponent &c) {
        c = {0};
        c.target = {0.0f, 0.0f};
        c.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        c.rotation = 0.0f;
        c.zoom = 1.0f;
    });

    use_debug_camera = false;

    debug_camera = {0};
    debug_camera.target = {0.0f, 0.0f};
    debug_camera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    debug_camera.rotation = 0.0f;
    debug_camera.zoom = 1.0f;

    // add the render system
    world.system().kind(flecs::PostUpdate).iter(render_system);

    // add the resource manager
    world.set<Resources>({});

    // misc
    regenerateGradientTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    // Image verticalGradient =
    //    GenImageGradientV(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5, RED, YELLOW);
    // sprite_tex = LoadTextureFromImage(verticalGradient);
    // world.get_mut<Resources>()->textures.Load;

    // add the camera entity here for now
    /*  auto test_e = world.entity("TestEntity")
                        .set([&](SpriteComponent &c) {
                            c = {0};
                            c.resourceHandle =
                                world.get_mut<Resources>()->textures.Load(
                                    LoadTextureFromImage(verticalGradient));
                            c.width = 100;
                            c.height = 100;
                        })
                        .set(([&](Position &c) {
                            c.x = 0;
                            c.y = 0;
                        }))
                        .set([&](CircleShapeRenderComponent &c) { c.radius
       = 25.0f;
                        });*/
}

void destroy() {
    CloseWindow();
    UnloadTexture(gradient_tex);
}

} // namespace graphics
