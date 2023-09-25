#include "render_systems.h"
#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "../components/render_components.h"
#include "flecs.h"
#include "raymath.h"
#include "rlgl.h"
#include <filesystem>
#include <iostream>

namespace graphics {

Music ambient_audio;

Texture2D gradientTex;

Texture2D background_tex;
Texture2D midground_tex;
Texture2D foreground_tex;

// Initialize the scrolling speed
float scrollingBack = 0.0f;
float scrollingMid = 0.0f;
float scrollingFore = 0.0f;

HANDLE spriteTex;

int rotation = 0;

bool useDebugCamera;
Camera2D debugCamera;
Camera3D debugCamera3D;
Model model;
Shader shader;
Mesh grass_mesh;
Texture2D grass_texture;
std::vector<Matrix> transforms;
Material matInstances;
int loc_time;

void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradientTex); // TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradientTex = LoadTextureFromImage(verticalGradient);
    UnloadImage(verticalGradient);
}

float getTerrainHeight(float x, float y, float ridge_height,
                       float baseline = 0.0f) {
    const float scale = 0.01f;

    float distance_from_baseline = std::abs(y - baseline);

    float falloff = -(y * y) * scale;

    return (ridge_height + falloff);
}





void handleWindow(flecs::world &world) {
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
        // switch Appinfo isRunning to false;
        auto info = world.get_mut<AppInfo>();
        info->isRunning = false;
    }
}

void renderBackground(flecs::world &world, float cameraX, float cameraY) {
    DrawTexture(gradientTex, 0, 0, WHITE);

    // Update the scrolling speed
    scrollingBack -= 0.1f;
    scrollingMid -= 0.5f;
    scrollingFore -= 1.0f;

    if (scrollingBack <= -background_tex.width * 2)
        scrollingBack = 0;
    if (scrollingMid <= -midground_tex.width * 2)
        scrollingMid = 0;
    if (scrollingFore <= -foreground_tex.width * 2)
        scrollingFore = 0;

    world.entity("Background").get_mut<Position>()->x =
        scrollingBack + cameraX;
    world.entity("Midground").get_mut<Position>()->x =
        scrollingMid + cameraX;
    world.entity("Foreground").get_mut<Position>()->x =
        scrollingFore + cameraX;

    world.entity("BackgroundDuplicate").get_mut<Position>()->x =
        scrollingBack + graphics::SCREEN_WIDTH + cameraX;
    world.entity("MidgroundDuplicate").get_mut<Position>()->x =
        scrollingMid + graphics::SCREEN_WIDTH + cameraX;
    world.entity("ForegroundDuplicate").get_mut<Position>()->x =
        scrollingFore + graphics::SCREEN_WIDTH + cameraX;

    world.entity("Background").get_mut<Position>()->y =
        -(-graphics::SCREEN_HEIGHT * 0.25 + cameraY);
    world.entity("Midground").get_mut<Position>()->y =
        -(-graphics::SCREEN_HEIGHT * 0.25 + cameraY);
    world.entity("Foreground").get_mut<Position>()->y =
        -(-graphics::SCREEN_HEIGHT * 0.25 + cameraY);

    world.entity("BackgroundDuplicate").get_mut<Position>()->y =
        -(-graphics::SCREEN_HEIGHT * 0.25 + cameraY);
    world.entity("MidgroundDuplicate").get_mut<Position>()->y =
        -(-graphics::SCREEN_HEIGHT * 0.25 + cameraY);
    world.entity("ForegroundDuplicate").get_mut<Position>()->y =
        -(-graphics::SCREEN_HEIGHT * 0.25 + cameraY);
}


void render_system(flecs::iter &iter) {
    auto world = iter.world();

    UpdateMusicStream(ambient_audio);

    handleWindow(world);

    if (IsKeyPressed(KEY_P)) {
        useDebugCamera = !useDebugCamera;
    }

    auto camera_entity = world.lookup("Camera");

    if (camera_entity.is_valid()) {
        auto camera = camera_entity.get_mut<Camera2DComponent>();

        // Update the light shader with the camera view position
        float cameraPos[3] = {debugCamera3D.position.x,
                              debugCamera3D.position.y,
                              debugCamera3D.position.z};
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos,
                       SHADER_UNIFORM_VEC3);

        // if (useDebugCamera) {
        //     if (IsKeyDown(KEY_D))
        //         debugCamera.target.x += 2;
        //     else if (IsKeyDown(KEY_A))
        //         debugCamera.target.x -= 2;
        //     else if (IsKeyDown(KEY_S))
        //         debugCamera.target.y += 2;
        //     else if (IsKeyDown(KEY_W))
        //         debugCamera.target.y -= 2;

        //    if (IsKeyDown(KEY_LEFT))
        //        debugCamera.rotation--;
        //    else if (IsKeyDown(KEY_RIGHT))
        //        debugCamera.rotation++;
        //}

        auto interval =
            world.get_mut<Mountain>()->getIndexIntervalOfEntireMountain();

        BeginDrawing();
        {
            ClearBackground(WHITE);
            renderBackground(world, camera->target.x, camera->target.y);
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

                // auto mountain = world.get_mut<Mountain>();
                // for (int i = interval.start_index; i < interval.end_index;
                //      i++) {

                //    Vector2 control_point_0{mountain->getVertex(i).x,
                //                            -mountain->getVertex(i).y};
                //    Vector2 control_point_1{mountain->getVertex(i + 1).x,
                //                            -mountain->getVertex(i + 1).y};
                //    Vector2 control_point_2{mountain->getVertex(i).x,
                //                            -mountain->getVertex(i).y};
                //    Vector2 control_point_3{mountain->getVertex(i + 1).x,
                //                            -mountain->getVertex(i + 1).y};

                //    DrawLineBezierCubic(control_point_0, control_point_1,
                //                        control_point_2, control_point_3, 5,
                //                        RED);
                //}

                //// Draw the control points and lines
                // if (DEBUG) {
                //     for (int i = interval.start_index; i <
                //     interval.end_index;
                //          i++) {
                //         Vector2 point = {mountain->getVertex(i).x,
                //                          -mountain->getVertex(i).y};

                //        DrawCircleV(point, 5,
                //                    BLUE); // Draw control points as circles
                //    }
                //}

                /* flecs::filter<Position, CircleShapeRenderComponent> cirle_q =
                     world.filter<Position, CircleShapeRenderComponent>();

                 cirle_q.each([&](Position &p, CircleShapeRenderComponent &s) {
                     DrawCircleLines(p.x, -p.y, s.radius, GREEN);
                 });*/

                /*flecs::filter<Position, RectangleShapeRenderComponent>
                    rectangle_q =
                        world.filter<Position, RectangleShapeRenderComponent>();

                rectangle_q.each(
                    [&](Position &p, RectangleShapeRenderComponent &s) {
                        DrawRectangle((int)p.x, (int)-p.y, (int)s.width,
                                      (int)s.height, RED);
                    });

                rotation++;*/
            }

            EndMode2D();

            if (useDebugCamera) {

                static float rotZ = 0;

                if (IsKeyDown(KEY_D))
                    debugCamera3D.position.x += 100 * iter.delta_time();
                else if (IsKeyDown(KEY_A))
                    debugCamera3D.position.x -= 100 * iter.delta_time();
                else if (IsKeyDown(KEY_E))
                    debugCamera3D.position.z -= 100 * iter.delta_time();
                else if (IsKeyDown(KEY_Q))
                    debugCamera3D.position.z += 100 * iter.delta_time();
                else if (IsKeyDown(KEY_W))
                    debugCamera3D.position.y += 100 * iter.delta_time();
                else if (IsKeyDown(KEY_S))
                    debugCamera3D.position.y -= 100 * iter.delta_time();

                if (IsKeyDown(KEY_LEFT))
                    rotZ += 2 * iter.delta_time();
                else if (IsKeyDown(KEY_RIGHT))
                    rotZ -= 2 * iter.delta_time();

                debugCamera3D.target.x =
                    debugCamera3D.position.x +cosf(rotZ);
                debugCamera3D.target.y =
                    debugCamera3D.position.y +1.0 + sinf(rotZ);
                debugCamera3D.target.z = debugCamera3D.position.z;
            } else {
                
                debugCamera3D.position.x = camera->target.x - 200;
                debugCamera3D.position.z = -camera->target.y + 100;

                debugCamera3D.target.x = debugCamera3D.position.x;
                debugCamera3D.target.y = debugCamera3D.position.y + 1.0;
                debugCamera3D.target.z = debugCamera3D.position.z;
            }

            if (s_regenerateTerrain) {
                std::cout << "---> regen terrain" << std::endl;
                auto test = generate_chunk_mesh(world);
                model = LoadModelFromMesh(test);

                /*Shader shader_hill =
                    LoadShader("../assets/shaders/hill.vert",
                               "../assets/shaders/hill.frag");
                model.materials[0].shader = shader_hill;*/
                model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = gradientTex;

                /*int loc = GetShaderLocation(shader, "hilltex");
                SetShaderValueTexture(shader, loc, gradientTex);*/

                s_regenerateTerrain = false;
            }

            BeginMode3D(debugCamera3D);
            {
                //DrawModelWires(model, {0.0, 0.0}, 1.0f, GREEN);
                DrawModel(model, {0.0, 0.0}, 1.0f, WHITE);// GREEN);
                //DrawCube({-20, 0}, 10, 10, 10, RED);

                flecs::filter<Position, BillboardComponent> q =
                    world.filter<Position, BillboardComponent>();

                q.each([&](Position &p, BillboardComponent &b) {
                    if (b.resourceHandle != NULL_HANDLE) {
                        auto texture = world.get_mut<Resources>()->textures.Get(
                            b.resourceHandle);

                        Rectangle sourceRec = {
                            0.0f, 0.0f, (float)texture.width,
                            (float)texture.height}; // part of the texture used

                        Rectangle destRec = {
                            p.x, p.y, static_cast<float>(b.width),
                            static_cast<float>(
                                b.height)}; // where to draw texture
                        ;

                        DrawBillboardPro(debugCamera3D, texture, sourceRec,
                                         Vector3{p.x, -500.0f, p.y}, b.billUp,
                                         Vector2{static_cast<float>(b.width),
                                                 static_cast<float>(b.width)},
                                         Vector2{0.0f, 0.0f}, 0.0f, WHITE);
                    }
                });

                rlDisableBackfaceCulling();
                matInstances.maps[MATERIAL_MAP_DIFFUSE].texture = grass_texture;
                static float elapsed_time = 0.0f;
                elapsed_time += iter.delta_time();
                SetShaderValue(shader, loc_time, &elapsed_time,
                               SHADER_UNIFORM_FLOAT);
                int count = std::min((int)transforms.size(), MAX_INSTANCES);
                DrawMeshInstanced(grass_mesh, matInstances, transforms.data(),
                                  count);
                rlEnableBackfaceCulling();

                // draw rocks
                flecs::filter<Position, CircleShapeRenderComponent> cirle_q =
                    world.filter<Position, CircleShapeRenderComponent>();

                cirle_q.each([&](Position &p, CircleShapeRenderComponent &s) {
                    DrawSphere(
                        {p.x - s.radius / 2, -s.radius / 2, p.y - s.radius / 2},
                        s.radius, GREEN);
                });

                auto mountain = world.get_mut<Mountain>();

                // draw player
                flecs::filter<Position, RectangleShapeRenderComponent>
                    rectangle_q =
                        world.filter<Position, RectangleShapeRenderComponent>();

                rectangle_q.each(
                    [&](Position &p, RectangleShapeRenderComponent &s) {
                        /*DrawCube({p.x, -0.5, p.y},
                                 s.width, 1.0,
                                 s.height, BLUE);*/
                        DrawCube({p.x, -0.5, p.y - s.height / 2}, s.width, 1.0,
                                 s.height, RED);
                        /*DrawCube({p.x - s.width / 4, 0, p.y - s.height / 4},
                                 s.width / 2, 1.0,
                                 s.height / 2, BLUE);*/
                    });
            }
            EndMode3D();
        }
        EndDrawing();
    }
}

void init_render_system(flecs::world &world) {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME);
    InitAudioDevice();

    background_tex = LoadTexture("../assets/layers/sky.png");
    midground_tex = LoadTexture("../assets/layers/glacial_mountains.png");
    foreground_tex = LoadTexture("../assets/layers/clouds_mg_1.png");

    ambient_audio = LoadMusicStream("../assets/audio/sandstorm.mp3");
    PlayMusicStream(ambient_audio);

    /*    AudioComponent* audioComponent =
       world.entity("AmbientSound").get_mut<AudioComponent>(); Resources*
       resources = world.entity("AmbientSound").get_mut<Resources>();

        if (audioComponent && resources) {
            PlayMusicStream(resources->music.Get(audioComponent->resourceHandle));
        }*/

    // PlayMusicStream(world.entity("AmbientSound").get_mut<Resources>()->music.Get(world.entity("AmbientSound").get_mut<AudioComponent>()->resourceHandle));

    // add the camera entity here for now
    auto camera = world.entity("Camera").set([](Camera2DComponent &c) {
        c = {0};
        c.target = {0.0f, 0.0f};
        c.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        c.rotation = 0.0f;
        c.zoom = 1.0f;
    });

    useDebugCamera = false;

    debugCamera = {0};
    debugCamera.target = {0.0f, 0.0f};
    debugCamera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    debugCamera.rotation = 0.0f;
    debugCamera.zoom = 1.0f;

    // add the render system
    world.system().kind(flecs::PostUpdate).iter(render_system);

    // add the resource manager
    world.set<Resources>({});

    // misc
    regenerateGradientTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    Image verticalGradient =
        GenImageGradientV(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5, RED, YELLOW);
    // spriteTex = LoadTextureFromImage(verticalGradient);
    // world.get_mut<Resources>()->textures.Load;

    // add the camera entity here for now
    auto test_e =
        world.entity("TestEntity")
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
            }))
            .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f; });

    auto background =
        world.entity("Background")
            .set([&](SpriteComponent &c) {
                c = {0};
                c.resourceHandle =
                    world.get_mut<Resources>()->textures.Load(background_tex);
                c.width = SCREEN_WIDTH;
                c.height = SCREEN_HEIGHT;
            })
            .set(([&](Position &c) {
                c.x = 0;
                c.y = 0;
            }))
            .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f; });

    auto background_duplicate =
        world.entity("BackgroundDuplicate")
            .set([&](SpriteComponent &c) {
                c = {0};
                c.resourceHandle =
                    world.get_mut<Resources>()->textures.Load(background_tex);
                c.width = SCREEN_WIDTH;
                c.height = SCREEN_HEIGHT;
            })
            .set(([&](Position &c) {
                c.x = 0;
                c.y = 0;
            }))
            .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f; });

    auto midground =
        world.entity("Midground")
            .set([&](SpriteComponent &c) {
                c = {0};
                c.resourceHandle =
                    world.get_mut<Resources>()->textures.Load(midground_tex);
                c.width = SCREEN_WIDTH;
                c.height = SCREEN_HEIGHT;
            })
            .set(([&](Position &c) {
                c.x = 0;
                c.y = 0;
            }))
            .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f; });

    auto midground_duplicate =
        world.entity("MidgroundDuplicate")
            .set([&](SpriteComponent &c) {
                c = {0};
                c.resourceHandle =
                    world.get_mut<Resources>()->textures.Load(midground_tex);
                c.width = SCREEN_WIDTH;
                c.height = SCREEN_HEIGHT;
            })
            .set(([&](Position &c) {
                c.x = 0;
                c.y = 0;
            }))
            .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f; });

    auto foreground =
        world.entity("Foreground")
            .set([&](SpriteComponent &c) {
                c = {0};
                c.resourceHandle =
                    world.get_mut<Resources>()->textures.Load(foreground_tex);
                c.width = SCREEN_WIDTH;
                c.height = SCREEN_HEIGHT;
            })
            .set(([&](Position &c) {
                c.x = 0;
                c.y = 0;
            }))
            .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f; });
    auto foreground_duplicate =
        world.entity("ForegroundDuplicate")
            .set([&](SpriteComponent &c) {
                c = {0};
                c.resourceHandle =
                    world.get_mut<Resources>()->textures.Load(foreground_tex);
                c.width = SCREEN_WIDTH;
                c.height = SCREEN_HEIGHT;
            })
            .set(([&](Position &c) {
                c.x = 0;
                c.y = 0;
            }))
            .set([&](CircleShapeRenderComponent &c) { c.radius = 25.0f; });

    auto ambient_sound =
        world.entity("AmbientSound").set([&](AudioComponent &c) {
            c = {0};
            c.resourceHandle =
                world.get_mut<Resources>()->music.Load(ambient_audio);
        });

    // billboard for 3d camera
    auto billboard = world.entity("Billboard")
                         .set([&](BillboardComponent &c) {
                             c = {0};
                             c.billUp = {0.0f, 0.0f, 1.0f};
                             c.billPositionStatic = {0.0f, 0.0f, 0.0f};
                             c.resourceHandle =
                                 world.get_mut<Resources>()->textures.Load(
                                     "../assets/texture/raylib_256x256.png");
                             c.width = 100;
                             c.height = 100;
                         })
                         .set(([&](Position &c) {
                             c.x = 800;
                             c.y = 50;
                         }));

    Vector2 min;
    Vector2 max;
    /*auto test = generate_chunk_mesh(world);
    model = LoadModelFromMesh(test);*/
    // model = LoadModel("../../../assets/mesh/grass_patch.obj");
    // std::cout << "current paht " << std::filesystem::current_path() <<
    // std::endl;
    //  Load lighting shader
    shader = LoadShader("../assets/shaders/grass_instancing.vert",
                        "../assets/shaders/grass_instancing.frag");

    // Get shader locations
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocationAttrib(shader, "instanceTransform");

    // Set shader value: ambient light level
    int ambientLoc = GetShaderLocation(shader, "ambient");
    float col[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    SetShaderValue(shader, ambientLoc, &col[0], SHADER_UNIFORM_VEC4);

    // Define mesh to be instanced
    // grass_mesh = GenMeshCube(1.0f, 1.0f, 1.0f);

    Model grass_model = LoadModel("../assets/mesh/grass_patch.obj");
    grass_mesh = grass_model.meshes[0];

    // DrawModel

    Texture2D grass_texture = LoadTexture("../assets/texture/grass.png");

    int loc = GetShaderLocation(shader, "grasstex");
    loc_time = GetShaderLocation(shader, "time");
    SetShaderValueTexture(shader, loc, grass_texture);

    // Define transforms to be uploaded to GPU for instances
    transforms.reserve(MAX_INSTANCES);

    // Translate and rotate cubes randomly
    // for (int i = 0; i < MAX_INSTANCES; i++) {

    //    transforms.push_back(MatrixTranslate((float)GetRandomValue(-50, 50),
    //                                         (float)GetRandomValue(-50, 50),
    //                                         0));
    //}

    // NOTE: We are assigning the intancing shader to material.shader
    // to be used on mesh drawing with DrawMeshInstanced()
    matInstances = LoadMaterialDefault();
    matInstances.shader = shader;
    // matInstances.maps[MATERIAL_MAP_DIFFUSE].color = RED;
    matInstances.maps[MATERIAL_MAP_DIFFUSE].texture = grass_texture;

    /*Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    Texture2D texture = LoadTextureFromImage(checked);
    UnloadImage(checked);*/

    debugCamera3D = {0};
    debugCamera3D.position = {500.0f, -1000.0f, 0.0f}; // Camera position
    debugCamera3D.target = {0.0f, 1.0f, 0.0f}; // Camera looking at point
    debugCamera3D.up = {0.0f, 0.0f,
                        1.0f};  // Camera up vector (rotation towards target)
    debugCamera3D.fovy = 45.0f; // Camera field-of-view Y
    debugCamera3D.projection = CAMERA_PERSPECTIVE; // Camera mode type

    HANDLE test_texture = world.get_mut<Resources>()->textures.Load(
        "../assets/texture/grass.png"); // LoadTexture("../assets/texture/grass.png");

    world.get_mut<Resources>()->textures.Free(test_texture);
}

Vector3 compute_normal(Vector3 p1, Vector3 p2, Vector3 p3) {
    Vector3 n;

    auto Ax = p2.x - p1.x;
    auto Ay = p2.y - p1.y;
    auto Az = p2.z - p1.z;

    auto Bx = p3.x - p1.x;
    auto By = p3.y - p1.y;
    auto Bz = p3.z - p1.z;

    n.x = Ay * Bz - Az * By;
    n.y = Az * Bx - Ax * Bz;
    n.z = Ax * By - Ay * Bx;

    return n;
}

// Generate a simple triangle mesh from code
Mesh generate_chunk_mesh(flecs::world &world) {
    std::cout << "graphics: gen chunk" << std::endl;
    // world.get_mut<Mountain>()->generateNewChunk();

    auto interval2 =
        world.get_mut<Mountain>()->getIndexIntervalOfEntireMountain();

    auto interval =
        world.get_mut<Mountain>()->getLatestChunk();

    // interval.end_index =
    //     std::min(interval.end_index,
    //              interval.start_index +
    //                  1500); // TODO remove (last vertices are wrong...)

    int terrainVertexCount = interval.end_index - interval.start_index;

    int levels = 10;
    int levelsAtTheBack = 0; // number terrain layers behind the ridge

    int triangleCount = levels * (terrainVertexCount - 1) * 2;
    int vertexCount = triangleCount * 3;
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<float> normals;

    // for (int i = interval.start_index; i < interval.end_index-1; i++) {
    for (int i = interval.start_index; i < interval.end_index - 1; i++) {
        int currentDepth = -levelsAtTheBack * 0.1;

        const float x_scale = 0.1f;
        const float y_scale = 15.0f;

        float maxX = (interval.end_index - 1 - interval.start_index) * x_scale;
        float maxY = levels * y_scale;

        Vector3 v0;
        auto vertex =
            world.get_mut<Mountain>()->getVertex(interval.start_index + i);
        v0.x = vertex.x;
        v0.z = getTerrainHeight(vertex.x, currentDepth, vertex.y);
        v0.y = currentDepth;

        // std::cout << "vertex: " <<i << ": " << vertex.x << ", " << vertex.y
        // << std::endl;

        Vector3 v1;
        auto vertex2 =
            world.get_mut<Mountain>()->getVertex(interval.start_index + i + 1);
        // vertex2.x = vertex2.x * 0.01f;
        // vertex2.y = vertex2.y * 0.01f;
        //  getTerrainHeight(vertex.x, currentDepth, vertex.y);
        v1.x = vertex2.x;
        v1.z = getTerrainHeight(vertex2.x, currentDepth, vertex2.y); // height;
        v1.y = currentDepth;

        for (int level = 0; level < levels; level++) {
            auto v2 = v0; // in front of terrain vertex i-1
            v2.y -= y_scale;
            v2.z = getTerrainHeight(v2.x, v2.y, v2.z);
            auto v3 = v1; // in front of terrain vertex i
            v3.y -= y_scale;
            v3.z = getTerrainHeight(v3.x, v3.y, v3.z);

            // first triangle
            // v1, v0, v2
            auto normal = compute_normal(v1, v0, v2);

            vertices.push_back(v1.x);
            vertices.push_back(v1.y);
            vertices.push_back(v1.z);
            texcoords.push_back(v1.x / maxX);
            texcoords.push_back(v1.y / maxY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            vertices.push_back(v0.x);
            vertices.push_back(v0.y);
            vertices.push_back(v0.z);
            texcoords.push_back(v0.x / maxX);
            texcoords.push_back(v0.y / maxY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            vertices.push_back(v2.x);
            vertices.push_back(v2.y);
            vertices.push_back(v2.z); // below terrain vertex i-1
            texcoords.push_back(v2.x / maxX);
            texcoords.push_back(v2.y / maxY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            // second triangle
            // v1, v2, v3
            auto normal2 = compute_normal(v1, v2, v3);

            vertices.push_back(v1.x);
            vertices.push_back(v1.y);
            vertices.push_back(v1.z);
            texcoords.push_back(v1.x / maxX);
            texcoords.push_back(v1.y / maxY);
            normals.push_back(normal2.x);
            normals.push_back(normal2.y);
            normals.push_back(normal2.z);

            vertices.push_back(v2.x);
            vertices.push_back(v2.y);
            vertices.push_back(v2.z);
            texcoords.push_back(v2.x / maxX);
            texcoords.push_back(v2.y / maxY);
            normals.push_back(normal2.x);
            normals.push_back(normal2.y);
            normals.push_back(normal2.z);

            vertices.push_back(v3.x);
            vertices.push_back(v3.y);
            vertices.push_back(v3.z); // below terrain vertex i
            texcoords.push_back(v3.x / maxX);
            texcoords.push_back(v3.y / maxY);
            normals.push_back(normal2.x);
            normals.push_back(normal2.y);
            normals.push_back(normal2.z);

            // grass pos
            if (level > 2 && rand() % 15 == 0 /* && level < levels - 1 &&
                i < interval.end_index - 20*/) {

                float r0 = ((float) rand()) / RAND_MAX;
                //float r1 = rand() / RAND_MAX;
                //// random position in quad
                //auto v01 = Vector3Scale(Vector3Subtract(v0, v1), r0);
                //auto v02 = Vector3Scale(Vector3Subtract(v0, v2), r1);

                //auto position = Vector3Add(v0, Vector3Add(v01, v02));

                ////transforms.push_back(MatrixTranslate(position.x, position.y, position.z));
                //
                //auto translate = MatrixTranslate(position.x, position.y, position.z); // MatrixTranslate(v1.x, v1.y, v1.z);
                
                auto v12 = Vector3Subtract(v1, v2); // down
                auto translate = MatrixTranslate(v1.x, v1.y, v1.z + v12.z * r0);
                
                //auto translate = MatrixTranslate(v1.x, v1.y, v1.z);
                auto scale = MatrixScale(20.0, 20.0, 20.0);

                transforms.push_back(MatrixMultiply(scale, translate));     
            }

            // shift to the front
            v0 = v2;
            v1 = v3;
        }
    }

    Mesh mesh = {0};
    mesh.triangleCount = triangleCount;
    mesh.vertexCount = vertexCount;
    mesh.vertices = vertices.data(); // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = texcoords.data(); // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = normals.data(); // 3 vertices, 3 coordinates each (x, y, z)

    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, false);

    return mesh;
}

void destroy() {
    UnloadShader(shader);
    UnloadTexture(gradientTex);
    // UnloadModel(model);

    UnloadTexture(background_tex);
    UnloadTexture(midground_tex);
    UnloadTexture(foreground_tex);
    CloseAudioDevice();
    CloseWindow();
}

} // namespace graphics
