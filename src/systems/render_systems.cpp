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

Texture2D gradientTex;
HANDLE spriteTex;

int rotation = 0;

bool useDebugCamera;
Camera2D debugCamera;
Camera3D debugCamera3D;
Model model;

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



void render_system(flecs::iter& iter) {
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

        auto interval = world.get_mut<Mountain>()->getIndexIntervalOfEntireMountain();


        BeginDrawing();
        {
            ClearBackground(BLUE);
            DrawTexture(gradientTex, 0, 0, WHITE);

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

                    Vector2 control_point_0{mountain->getVertex(i).x, mountain->getVertex(i).y};
                    Vector2 control_point_1{mountain->getVertex(i+1).x, mountain->getVertex(i+1).y};
                    Vector2 control_point_2{mountain->getVertex(i).x, mountain->getVertex(i).y};
                    Vector2 control_point_3{mountain->getVertex(i+1).x, mountain->getVertex(i+1).y};

                    DrawLineBezierCubic(control_point_0, control_point_1, control_point_2,
                                        control_point_3, 5, RED);
                }

                // Draw the control points and lines
                if (DEBUG) {
                    for (int i = interval.start_index; i < interval.end_index;
                        i++) {
                        Vector2 point = {mountain->getVertex(i).x, mountain->getVertex(i).y};

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

                    Rectangle destRec = {p.x, p.y, static_cast<float>(s.width),
                                         static_cast<float>(s.height)}; // where to draw texture

                    DrawTexturePro(
                        texture, sourceRec, destRec,
                        {(float)texture.width, (float)texture.height}, 0,
                        WHITE);
                }
            });

            rotation++;
            }

            EndMode2D();

            if (useDebugCamera) {
                if (IsKeyDown(KEY_D))
                    debugCamera3D.position.x += 2;
                else if (IsKeyDown(KEY_A))
                    debugCamera3D.position.x -= 2;
                else if (IsKeyDown(KEY_S))
                    debugCamera3D.position.z -= 2;
                else if (IsKeyDown(KEY_W))
                    debugCamera3D.position.z += 2;

                if (IsKeyDown(KEY_LEFT))
                        debugCamera.rotation--;
                else if (IsKeyDown(KEY_RIGHT))
                        debugCamera.rotation++;

                debugCamera3D.target.x = debugCamera3D.position.x;
                debugCamera3D.target.z = debugCamera3D.position.z;
                debugCamera3D.target.y = 1;
            }
            //debugCamera3D.position = { debugCamera.target.x, -1.0, debugCamera.target.y};
            //debugCamera3D.target = {debugCamera.target.x,1.0,debugCamera.target.y};


            BeginMode3D(debugCamera3D);
            { 
                DrawModel(model, {0.0, 0.0}, 1.0f, RED);
                DrawCube({-20,0}, 10, 10, 10, RED);
            }
            EndMode3D();
        }
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
    //world.get_mut<Resources>()->textures.Load;

    // add the camera entity here for now
    auto test_e = world.entity("TestEntity")
                      .set([&](SpriteComponent &c) {
                          c = {0};
                          c.resourceHandle = world.get_mut<Resources>()->textures.Load(
                              LoadTextureFromImage(verticalGradient));
                          c.width = 100;
                          c.height = 100;
                      })
                      .set(([&](Position &c) {
                          c.x = 0;
                          c.y = 0;
                      }));

    
     model = LoadModelFromMesh(generate_chunk_mesh());

    debugCamera3D = {0};
    debugCamera3D.position = {0.0f, -10.0f, 0.0f}; // Camera position
    debugCamera3D.target = {0.0f, 0.0f, 0.0f}; // Camera looking at point
    debugCamera3D.up = {0.0f, 0.0f, 1.0f}; // Camera up vector (rotation towards target)
    debugCamera3D.fovy = 45.0f; // Camera field-of-view Y
    debugCamera3D.projection = CAMERA_PERSPECTIVE; // Camera mode type
}

// Generate a simple triangle mesh from code
Mesh generate_chunk_mesh() {
    Mesh mesh = {0};
    mesh.triangleCount = 1;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float *)MemAlloc(
        mesh.vertexCount * 3 *
        sizeof(float)); // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float *)MemAlloc(
        mesh.vertexCount * 2 *
        sizeof(float)); // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float *)MemAlloc(
        mesh.vertexCount * 3 *
        sizeof(float)); // 3 vertices, 3 coordinates each (x, y, z)

    // Vertex at (0, 0, 0)
    mesh.vertices[0] = 0;
    mesh.vertices[1] = 0;
    mesh.vertices[2] = 0;
    mesh.normals[0] = 0;
    mesh.normals[1] = 1;
    mesh.normals[2] = 0;
    mesh.texcoords[0] = 0;
    mesh.texcoords[1] = 0;

    // Vertex at (1, 0, 2)
    mesh.vertices[3] = 10;
    mesh.vertices[4] = 0;
    mesh.vertices[5] = 0;
    mesh.normals[3] = 0;
    mesh.normals[4] = 1;
    mesh.normals[5] = 0;
    mesh.texcoords[2] = 0.5f;
    mesh.texcoords[3] = 1.0f;

    // Vertex at (2, 0, 0)
    mesh.vertices[6] = 10;
    mesh.vertices[7] = 0;
    mesh.vertices[8] = 10;
    mesh.normals[6] = 0;
    mesh.normals[7] = 1;
    mesh.normals[8] = 0;
    mesh.texcoords[4] = 1;
    mesh.texcoords[5] = 0;

    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, false);

    return mesh;
}

void destroy() {
    CloseWindow();
    UnloadTexture(gradientTex);
}

} // namespace graphics
