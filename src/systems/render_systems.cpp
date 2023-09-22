#include "render_systems.h"
#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "../components/render_components.h"
// #include "../utils/resource_manager.h"
#include "flecs.h"
#include <iostream>

namespace graphics {

const int screenWidth = 800;
const int screenHeight = 450;


Music ambient_audio;

Texture2D gradientTex;

Texture2D background_tex;
Texture2D midground_tex;
Texture2D foreground_tex;

//Initialize the scrolling speed
float scrollingBack = 0.0f;
float scrollingMid = 0.0f;
float scrollingFore = 0.0f;

HANDLE spriteTex;

int rotation = 0;

bool useDebugCamera;
Camera2D debugCamera;

void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradientTex); // TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradientTex = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

void render_system(flecs::iter &iter) {
    auto world = iter.world();

    UpdateMusicStream(ambient_audio);

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

    if (IsKeyPressed(KEY_P)) {
        useDebugCamera = !useDebugCamera;
    }

    auto camera_entity = world.lookup("Camera");
    if (camera_entity.is_valid()) {
        auto camera = camera_entity.get_mut<Camera2DComponent>();

        if (useDebugCamera) {
            if (IsKeyDown(KEY_D))
                debugCamera.target.x += 2;
            else if (IsKeyDown(KEY_A))
                debugCamera.target.x -= 2;
            else if (IsKeyDown(KEY_S))
                debugCamera.target.y += 2;
            else if (IsKeyDown(KEY_W))
                debugCamera.target.y -= 2;

            if (IsKeyDown(KEY_LEFT))
                debugCamera.rotation--;
            else if (IsKeyDown(KEY_RIGHT))
                debugCamera.rotation++;
        }

        auto interval =
            world.get_mut<Mountain>()->getIndexIntervalOfEntireMountain();

        BeginDrawing();
        {
            //ClearBackground(BLUE);


            ClearBackground(WHITE);
            //DrawTexture(gradientTex, 0, 0, WHITE);

            //Update the scrolling speed
            scrollingBack -= 0.1f;
            scrollingMid -= 0.5f;
            scrollingFore -= 1.0f;

            if (scrollingBack <= -background_tex.width*2) scrollingBack = 0;
            if (scrollingMid <= -midground_tex.width*2) scrollingMid = 0;
            if (scrollingFore <= -foreground_tex.width*2) scrollingFore = 0;

            world.entity("Background").get_mut<Position>()->x = scrollingBack;
            world.entity("Midground").get_mut<Position>()->x = scrollingMid;
            world.entity("Foreground").get_mut<Position>()->x = scrollingFore;

            world.entity("BackgroundDuplicate").get_mut<Position>()->x = scrollingBack + world.entity("Background").get_mut<SpriteComponent>()->width;
            world.entity("MidgroundDuplicate").get_mut<Position>()->x = scrollingMid + world.entity("Midground").get_mut<SpriteComponent>()->width;
            world.entity("ForegroundDuplicate").get_mut<Position>()->x = scrollingFore + world.entity("Foreground").get_mut<SpriteComponent>()->width;

            /*world.get_mut<Resources>()*/

            if (useDebugCamera) {
                BeginMode2D(debugCamera);
            } else {
                BeginMode2D(*camera);
            }
            {
                // DrawText("Congrats! You created your first window!", 190,
                // 200, 20, LIGHTGRAY);

                // loop for all sprites (sprite component + transform
                // compoenent)

                // loor for all
                auto mountain = world.get_mut<Mountain>();
                for (int i = interval.start_index; i < interval.end_index;
                     i++) {

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
                    for (int i = interval.start_index; i < interval.end_index;
                         i++) {
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

                        Rectangle sourceRec = {
                            0.0f, 0.0f, (float)texture.width,
                            (float)texture.height}; // part of the texture used

                        Rectangle destRec = {
                            p.x, -p.y, static_cast<float>(s.width),
                            static_cast<float>(
                                s.height)}; // where to draw texture

                        DrawTexturePro(
                            texture, sourceRec, destRec,
                            {(float)texture.width, (float)texture.height}, 0,
                            WHITE);
                    }
                });


            flecs::filter<Position, CircleShapeRenderComponent> cirle_q =
                world.filter<Position, CircleShapeRenderComponent>();

            cirle_q.each([&](Position &p, CircleShapeRenderComponent &s) {
                DrawCircleLines(p.x, -p.y, s.radius, GREEN);
            });


            rotation++;
        }


            EndMode2D();
        }
        EndDrawing();
    }
}

void init_render_system(flecs::world &world) {
    InitWindow(screenWidth, screenHeight, WINDOW_NAME);
    InitAudioDevice();


    background_tex = LoadTexture("../assets/layers/glacial_mountains.png");
    midground_tex = LoadTexture("../assets/layers/sky.png");
    foreground_tex = LoadTexture("../assets/layers/clouds_mg_1.png");

    ambient_audio = LoadMusicStream("../assets/audio/sandstorm.mp3");
    PlayMusicStream(ambient_audio);

/*    AudioComponent* audioComponent = world.entity("AmbientSound").get_mut<AudioComponent>();
    Resources* resources = world.entity("AmbientSound").get_mut<Resources>();

    if (audioComponent && resources) {
        PlayMusicStream(resources->music.Get(audioComponent->resourceHandle));
    }*/


    //PlayMusicStream(world.entity("AmbientSound").get_mut<Resources>()->music.Get(world.entity("AmbientSound").get_mut<AudioComponent>()->resourceHandle));






    // add the camera entity here for now
    auto camera = world.entity("Camera").set([](Camera2DComponent &c) {
        c = {0};
        c.target = {0.0f, 0.0f};
        c.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
        c.rotation = 0.0f;
        c.zoom = 1.0f;
    });

    useDebugCamera = false;

    debugCamera = {0};
    debugCamera.target = {0.0f, 0.0f};
    debugCamera.offset = {screenWidth / 2.0f, screenHeight / 2.0f};
    debugCamera.rotation = 0.0f;
    debugCamera.zoom = 1.0f;

    // add the render system
    world.system().kind(flecs::PostUpdate).iter(render_system);

    // add the resource manager
    world.set<Resources>({});

    // misc
    regenerateGradientTexture(screenWidth, screenHeight);
    Image verticalGradient =
        GenImageGradientV(screenWidth / 5, screenHeight / 5, RED, YELLOW);
    // spriteTex = LoadTextureFromImage(verticalGradient);
    // world.get_mut<Resources>()->textures.Load;

    // add the camera entity here for now
    auto test_e = world.entity("TestEntity")
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
                      .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f;
                      });

    auto midground = world.entity("Midground")
                      .set([&](SpriteComponent &c) {
                          c = {0};
                          c.resourceHandle =
                              world.get_mut<Resources>()->textures.Load(
                                  midground_tex);
                          c.width = screenWidth;
                          c.height = screenHeight;
                      })
                      .set(([&](Position &c) {
                          c.x = 0;
                          c.y = 0;
                      }))
                      .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f;
                      });

    auto midground_duplicate = world.entity("MidgroundDuplicate")
                         .set([&](SpriteComponent &c) {
                             c = {0};
                             c.resourceHandle =
                                 world.get_mut<Resources>()->textures.Load(
                                     midground_tex);
                             c.width = screenWidth;
                             c.height = screenHeight;
                         })
                         .set(([&](Position &c) {
                             c.x = 0;
                             c.y = 0;
                         }))
                         .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f;
                         });

    auto background = world.entity("Background")
                         .set([&](SpriteComponent &c) {
                             c = {0};
                             c.resourceHandle =
                                 world.get_mut<Resources>()->textures.Load(
                                     background_tex);
                             c.width = screenWidth;
                             c.height = screenHeight;
                         })
                         .set(([&](Position &c) {
                             c.x = 0;
                             c.y = 0;
                         }))
                         .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f;
                         });

    auto background_duplicate = world.entity("BackgroundDuplicate")
                          .set([&](SpriteComponent &c) {
                              c = {0};
                              c.resourceHandle =
                                  world.get_mut<Resources>()->textures.Load(
                                      background_tex);
                              c.width = screenWidth;
                              c.height = screenHeight;
                          })
                          .set(([&](Position &c) {
                              c.x = 0;
                              c.y = 0;
                          }))
                          .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f;
                          });

    auto foreground = world.entity("Foreground")
                          .set([&](SpriteComponent &c) {
                              c = {0};
                              c.resourceHandle =
                                  world.get_mut<Resources>()->textures.Load(
                                      foreground_tex);
                              c.width = screenWidth;
                              c.height = screenHeight;
                          })
                          .set(([&](Position &c) {
                              c.x = 0;
                              c.y = 0;
                          }))
                          .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f;
                          });
    auto foreground_duplicate = world.entity("ForegroundDuplicate")
                          .set([&](SpriteComponent &c) {
                              c = {0};
                              c.resourceHandle =
                                  world.get_mut<Resources>()->textures.Load(
                                      foreground_tex);
                              c.width = screenWidth;
                              c.height = screenHeight;
                          })
                          .set(([&](Position &c) {
                              c.x = 0;
                              c.y = 0;
                          }))
                          .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f;
                          });

    auto ambient_sound = world.entity("AmbientSound")
                                    .set([&](AudioComponent &c) {
                                        c = {0};
                                        c.resourceHandle =
                                            world.get_mut<Resources>()->music.Load(ambient_audio);
                                    });
}

void destroy() {
    UnloadTexture(gradientTex);
    UnloadTexture(background_tex);
    UnloadTexture(midground_tex);
    UnloadTexture(foreground_tex);
    CloseAudioDevice();
    CloseWindow();
}

} // namespace graphics
