#include "render_systems.h"
#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "../components/player.h"
#include "../components/render_components.h"
#include "flecs.h"
#include "raymath.h"
#include "rlgl.h"
#include <filesystem>
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace graphics {

// Audio
Music ambient_audio;

// Background
Texture2D gradient_texture_background;
Texture2D background_tex;
Texture2D midground_tex;
Texture2D foreground_tex;

// Initialize the scrolling speed
float scrolling_back = 0.0f;
float scrolling_mid = 0.0f;
float scrolling_fore = 0.0f;

// Camera
bool use_debug_camera;
Camera2D debug_camera;
Camera3D debug_camera3D;

// Mountain
const int NUM_CHUNKS = 1000;
int next_mountain_replace = 0;
std::array<Model, NUM_CHUNKS> mountain_model;

// Grass
#define MAX_INSTANCES 1000
Shader grass_shader;
Mesh grass_mesh;
Texture2D grass_texture;
std::vector<Matrix> grass_transforms;
Material grass_material;

// frame info
int current_frame = 0;
int loc_time;
bool regenerate_terrain = true;
int grass_insert_index = 0;

// Phases
flecs::entity StartRender;
flecs::entity OnRender;
flecs::entity OnInterface;
flecs::entity EndRender;

/*helpers*/

void regenerateGradientTexture(int screenW, int screenH) {
    UnloadTexture(gradient_texture_background); // TODO necessary?
    Image verticalGradient = GenImageGradientV(screenW, screenH, BLUE, WHITE);
    gradient_texture_background = LoadTextureFromImage(verticalGradient);
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
    DrawTexture(gradient_texture_background, 0, 0, WHITE);

    // Update the scrolling speed
    scrolling_back -= 0.1f;
    scrolling_mid -= 0.5f;
    scrolling_fore -= 1.0f;

    if (scrolling_back <= -background_tex.width * 2)
        scrolling_back = 0;
    if (scrolling_mid <= -midground_tex.width * 2)
        scrolling_mid = 0;
    if (scrolling_fore <= -foreground_tex.width * 2)
        scrolling_fore = 0;

    world.entity("Background").get_mut<Position>()->x =
        scrolling_back + cameraX;
    world.entity("Midground").get_mut<Position>()->x = scrolling_mid + cameraX;
    world.entity("Foreground").get_mut<Position>()->x =
        scrolling_fore + cameraX;

    world.entity("BackgroundDuplicate").get_mut<Position>()->x =
        scrolling_back + graphics::SCREEN_WIDTH + cameraX;
    world.entity("MidgroundDuplicate").get_mut<Position>()->x =
        scrolling_mid + graphics::SCREEN_WIDTH + cameraX;
    world.entity("ForegroundDuplicate").get_mut<Position>()->x =
        scrolling_fore + graphics::SCREEN_WIDTH + cameraX;

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

Vector3 computeNormal(Vector3 p1, Vector3 p2, Vector3 p3) {
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
void generateChunkMesh(const flecs::world &world) {
    // world.get_mut<Mountain>()->generateNewChunk();

    auto interval2 =
        world.get_mut<Mountain>()->getIndexIntervalOfEntireMountain();

    auto interval = world.get_mut<Mountain>()->getLatestChunk();

    std::cout << "graphics: gen chunk: " << interval.start_index << ", "
              << interval.end_index << std::endl;

    int start_index = interval.start_index - 1;
    int end_index = interval.end_index;

    int terrainVertexCount = end_index - start_index;

    int levels = 10;
    int levelsAtTheBack = 0; // number terrain layers behind the ridge

    int triangleCount = levels * (terrainVertexCount - 1) * 2;
    int vertexCount = triangleCount * 3;
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<float> normals;

    for (int i = start_index; i < end_index - 1; i++) {
        int currentDepth = -levelsAtTheBack * 0.1;

        const float x_scale = 0.1f;
        const float y_scale = 15.0f;

        float maxX = (end_index - 1 - start_index) * x_scale;
        float maxY = levels * y_scale;

        Vector3 v0;
        auto vertex = world.get_mut<Mountain>()->getVertex(i);
        v0.x = vertex.x;
        v0.z = getTerrainHeight(vertex.x, currentDepth, vertex.y);
        v0.y = currentDepth;

        Vector3 v1;
        auto vertex2 = world.get_mut<Mountain>()->getVertex(i + 1);
        v1.x = vertex2.x;
        v1.z = getTerrainHeight(vertex2.x, currentDepth, vertex2.y); // height;
        v1.y = currentDepth;

        if (i == start_index) {
            std::cout << "START IIIIIII " << v0.x << ", " << start_index
                      << std::endl;
        }

        if (interval.end_index - 2 == i) {
            std::cout << "END IIIIIII " << v1.x << ", " << end_index
                      << std::endl;
        }
        for (int level = 0; level < levels; level++) {
            auto v2 = v0; // in front of terrain vertex i-1
            v2.y -= y_scale;
            v2.z = getTerrainHeight(v2.x, v2.y, v2.z);
            auto v3 = v1; // in front of terrain vertex i
            v3.y -= y_scale;
            v3.z = getTerrainHeight(v3.x, v3.y, v3.z);

            // first triangle
            // v1, v0, v2
            auto normal = computeNormal(v1, v0, v2);

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
            auto normal2 = computeNormal(v1, v2, v3);

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

                float r0 = ((float)rand()) / RAND_MAX;
                // float r1 = rand() / RAND_MAX;
                //// random position in quad
                // auto v01 = Vector3Scale(Vector3Subtract(v0, v1), r0);
                // auto v02 = Vector3Scale(Vector3Subtract(v0, v2), r1);

                // auto position = Vector3Add(v0, Vector3Add(v01, v02));

                ////grass_transforms.push_back(MatrixTranslate(position.x,
                /// position.y,
                /// position.z));
                //
                // auto translate = MatrixTranslate(position.x, position.y,
                // position.z); // MatrixTranslate(v1.x, v1.y, v1.z);

                auto v12 = Vector3Subtract(v1, v2); // down
                auto translate = MatrixTranslate(v1.x, v1.y, v1.z + v12.z * r0);

                float scale = 20.0;
                translate.m0 = scale;
                translate.m5 = scale;
                translate.m10 = scale;

                if (grass_transforms.size() >= MAX_INSTANCES) { // TODO improve?
                    grass_transforms[grass_insert_index] = translate;
                } else {
                    grass_transforms.push_back(translate);
                }
                grass_insert_index++;

                if (grass_insert_index >= MAX_INSTANCES) {
                    grass_insert_index = 0;
                }
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

    //
    if (mountain_model[next_mountain_replace].meshes != nullptr) {
        for (int i = 0; i < mountain_model[next_mountain_replace].meshCount;
             i++) {
            // rlUnloadVertexBuffer(mountain_model[next_mountain_replace].meshes[0].vaoId);
            rlUnloadVertexArray(
                mountain_model[next_mountain_replace].meshes[i].vaoId);
            if (mesh.vboId != NULL)
                for (int j = 0; j < 7; j++)
                    rlUnloadVertexBuffer(mountain_model[next_mountain_replace]
                                             .meshes[i]
                                             .vboId[j]);
        }
    }

    // UnloadMesh();

    std::cout << "---> regen terrain" << std::endl;
    mountain_model[next_mountain_replace] = LoadModelFromMesh(mesh);

    // UnloadModel(mountain_model[next_mountain_replace]);// since q

    /*Shader shader_hill =
        LoadShader("../assets/shaders/hill.vert",
                    "../assets/shaders/hill.frag");
    model.materials[0].grass_shader = shader_hill;*/
    mountain_model[next_mountain_replace]
        .materials[0]
        .maps[MATERIAL_MAP_DIFFUSE]
        .texture = gradient_texture_background;

    next_mountain_replace = (next_mountain_replace + 1) % NUM_CHUNKS;

    /*int loc = GetShaderLocation(grass_shader, "hilltex");
    SetShaderValueTexture(grass_shader, loc, gradient_texture_background);*/
}

// init

void initRenderSystem(const flecs::world &world) {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME);
    InitAudioDevice();

    // setup custom phases
    StartRender =
        world.entity().add(flecs::Phase).depends_on(flecs::PostUpdate);

    OnRender = world.entity().add(flecs::Phase).depends_on(StartRender);

    OnInterface = world.entity().add(flecs::Phase).depends_on(OnRender);

    EndRender = world.entity().add(flecs::Phase).depends_on(OnInterface);

    // add the render system
    world.system().kind(StartRender).iter(startRender);

    // add the hud system
    world.system().kind(EndRender).iter(endRender);
}

// prepare

void prepareGameResources(const flecs::world &world) {

    // add the render system
    world.system().kind(OnRender).iter(renderSystem);
    // add the hud system
    world.system().kind(OnInterface).iter(renderHUD);

    // resources
    background_tex = LoadTexture("../assets/layers/sky.png");
    midground_tex = LoadTexture("../assets/layers/glacial_mountains.png");
    foreground_tex = LoadTexture("../assets/layers/clouds_mg_1.png");

    ambient_audio = LoadMusicStream("../assets/audio/sandstorm.mp3");
    PlayMusicStream(ambient_audio);

    // add the camera entity here for now
    auto camera = world.entity("Camera").set([](Camera2DComponent &c) {
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

    // add the resource manager
    world.set<Resources>({});

    // misc
    regenerateGradientTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    Image verticalGradient =
        GenImageGradientV(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5, RED, YELLOW);

    // spawn entities
    {
        // add the camera entity here for now
        auto test_e =
            world.entity("TestEntity")
                .set([&](SpriteComponent &c) {
                    c = {0};
                    c.resourceHandle =
                        world.get_mut<Resources>()->textures.load(
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
                        world.get_mut<Resources>()->textures.load(
                            background_tex);
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
                        world.get_mut<Resources>()->textures.load(
                            background_tex);
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
                        world.get_mut<Resources>()->textures.load(
                            midground_tex);
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
                        world.get_mut<Resources>()->textures.load(
                            midground_tex);
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
                        world.get_mut<Resources>()->textures.load(
                            foreground_tex);
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
                        world.get_mut<Resources>()->textures.load(
                            foreground_tex);
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
                    world.get_mut<Resources>()->music.load(ambient_audio);
            });

        // billboard for 3d camera
        auto billboard =
            world.entity("Billboard")
                .set([&](BillboardComponent &c) {
                    c = {0};
                    c.billUp = {0.0f, 0.0f, 1.0f};
                    c.billPositionStatic = {0.0f, 0.0f, 0.0f};
                    c.resourceHandle =
                        world.get_mut<Resources>()->textures.load(
                            "../assets/texture/raylib_256x256.png");
                    c.width = 100;
                    c.height = 100;
                })
                .set(([&](Position &c) {
                    c.x = 800;
                    c.y = 50;
                }));

        // billboard with animated sprite for 3d camera
        /*auto animatedBillboard = world.entity("AnimatedBillboard")
                             .set([&](AnimatedBillboardComponent &c) {
                                 c = {0};
                                 c.billUp = {0.0f, 0.0f, 1.0f};
                                 c.billPositionStatic = {0.0f, 0.0f, 0.0f};
                                 c.resourceHandle =
                                     world.get_mut<Resources>()->textures.Load(
                                         "../assets/texture/test_sprite_small.png");
                                 c.width = 100;
                                 c.height = 100;
                                 c.current_frame = 0;
                                 c.numFrames = 6;
                             })
                             .set(([&](Position &c) {
                                 c.x = 800;
                                 c.y = 300;
                             }));*/
    }

    // Grass
    {

        //  Load lighting grass_shader
        grass_shader = LoadShader("../assets/shaders/"
                                  "grass_instancing.vert",
                                  "../assets/shaders/"
                                  "grass_instancing.frag");

        // Get grass_shader locations
        grass_shader.locs[SHADER_LOC_MATRIX_MVP] =
            GetShaderLocation(grass_shader, "mvp");
        grass_shader.locs[SHADER_LOC_VECTOR_VIEW] =
            GetShaderLocation(grass_shader, "viewPos");
        grass_shader.locs[SHADER_LOC_MATRIX_MODEL] =
            GetShaderLocationAttrib(grass_shader, "instanceTransform");

        // Set grass_shader value: ambient
        // light level
        int ambientLoc = GetShaderLocation(grass_shader, "ambient");
        float col[4] = {0.2f, 0.2f, 0.2f, 1.0f};
        SetShaderValue(grass_shader, ambientLoc, &col[0], SHADER_UNIFORM_VEC4);

        // Define mesh to be instanced
        Model grass_model = LoadModel("../assets/mesh/"
                                      "grass_patch.obj");
        grass_mesh = grass_model.meshes[0];

        Texture2D grass_texture = LoadTexture("../assets/texture/"
                                              "grass.png");

        int loc = GetShaderLocation(grass_shader, "grasstex");
        loc_time = GetShaderLocation(grass_shader, "time");
        SetShaderValueTexture(grass_shader, loc, grass_texture);

        // transforms
        grass_transforms.reserve(MAX_INSTANCES);

        // materail
        grass_material = LoadMaterialDefault();
        grass_material.shader = grass_shader;
        grass_material.maps[MATERIAL_MAP_DIFFUSE].texture = grass_texture;
    }

    // camera setup
    {
        debug_camera3D = {0};
        debug_camera3D.position = {500.0f, -1000.0f, 0.0f}; // Camera position
        debug_camera3D.target = {0.0f, 1.0f, 0.0f}; // Camera looking at point
        debug_camera3D.up = {
            0.0f, 0.0f, 1.0f}; // Camera up vector (rotation towards target)
        debug_camera3D.fovy = 45.0f; // Camera field-of-view Y
        debug_camera3D.projection = CAMERA_PERSPECTIVE; // Camera mode type
    }
}

void prepareMenuResources(const flecs::world &world) {}

void startRender(const flecs::iter &iter) {
    // std::cout << "### start render" << std::endl;
    auto world = iter.world();
    handleWindow(world);
    BeginDrawing();
    ClearBackground(WHITE);
}

void endRender(const flecs::iter &iter) {
    EndDrawing();

    // std::cout << "### end render ----------------" << std::endl;
}

// draw

void renderSystem(const flecs::iter &iter) {

    // std::cout << "### render system" << std::endl;

    auto world = iter.world();

    UpdateMusicStream(ambient_audio);

    if (IsKeyPressed(KEY_P)) {
        use_debug_camera = !use_debug_camera;
    }

    auto camera_entity = world.lookup("Camera");

    if (camera_entity.is_valid()) {
        auto camera = camera_entity.get_mut<Camera2DComponent>();

        // Update the light grass_shader with the camera view position
        float cameraPos[3] = {debug_camera3D.position.x,
                              debug_camera3D.position.y,
                              debug_camera3D.position.z};
        SetShaderValue(grass_shader, grass_shader.locs[SHADER_LOC_VECTOR_VIEW],
                       cameraPos, SHADER_UNIFORM_VEC3);

        {
            renderBackground(world, camera->target.x, camera->target.y);
            /*world.get_mut<Resources>()*/

            if (use_debug_camera) {
                BeginMode2D(debug_camera);
            } else {
                BeginMode2D(*camera);
            }
            {

                flecs::filter<Position, SpriteComponent> q =
                    world.filter<Position, SpriteComponent>();

                q.each([&](Position &p, SpriteComponent &s) {
                    if (s.resourceHandle != NULL_HANDLE) {
                        auto texture = world.get_mut<Resources>()->textures.get(
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
            }

            EndMode2D();
            current_frame++;
            if (current_frame > 5)
                current_frame = 0;

            if (use_debug_camera) {

                static float rotZ = 0;

                if (IsKeyDown(KEY_D))
                    debug_camera3D.position.x += 100 * iter.delta_time();
                else if (IsKeyDown(KEY_A))
                    debug_camera3D.position.x -= 100 * iter.delta_time();
                else if (IsKeyDown(KEY_E))
                    debug_camera3D.position.z -= 100 * iter.delta_time();
                else if (IsKeyDown(KEY_Q))
                    debug_camera3D.position.z += 100 * iter.delta_time();
                else if (IsKeyDown(KEY_W))
                    debug_camera3D.position.y += 100 * iter.delta_time();
                else if (IsKeyDown(KEY_S))
                    debug_camera3D.position.y -= 100 * iter.delta_time();

                if (IsKeyDown(KEY_LEFT))
                    rotZ += 2 * iter.delta_time();
                else if (IsKeyDown(KEY_RIGHT))
                    rotZ -= 2 * iter.delta_time();

                debug_camera3D.target.x =
                    debug_camera3D.position.x + cosf(rotZ);
                debug_camera3D.target.y =
                    debug_camera3D.position.y + 1.0 + sinf(rotZ);
                debug_camera3D.target.z = debug_camera3D.position.z;
            } else {

                if (world.get_mut<AppInfo>()->playerAlive) {
                    debug_camera3D.position.x = camera->target.x - 200;
                    debug_camera3D.position.z = -camera->target.y + 100;

                    debug_camera3D.target.x = debug_camera3D.position.x;
                    debug_camera3D.target.y = debug_camera3D.position.y + 1.0;
                    debug_camera3D.target.z = debug_camera3D.position.z;
                }
            }

            BeginMode3D(debug_camera3D);
            {
                for (int i = 0; i < NUM_CHUNKS; i++) {
                    // DrawModelWires(model, {0.0, 0.0}, 1.0f, GREEN);
                    DrawModel(mountain_model[i], {0.0, 0.0}, 1.0f,
                              WHITE); // GREEN);
                }

                flecs::filter<Position, BillboardComponent> qb =
                    world.filter<Position, BillboardComponent>();

                qb.each([&](Position &p, BillboardComponent &b) {
                    if (b.resourceHandle != NULL_HANDLE) {
                        auto texture = world.get_mut<Resources>()->textures.get(
                            b.resourceHandle);

                        Rectangle sourceRec = {
                            0.0f, 0.0f, (float)texture.width,
                            (float)texture.height}; // part of the texture used

                        Rectangle destRec = {
                            p.x, p.y, static_cast<float>(b.width),
                            static_cast<float>(
                                b.height)}; // where to draw texture
                        ;

                        DrawBillboardPro(debug_camera3D, texture, sourceRec,
                                         Vector3{p.x + b.billPositionStatic.x,
                                                 0.0f + b.billPositionStatic.y,
                                                 p.y + +b.billPositionStatic.z},
                                         b.billUp,
                                         Vector2{static_cast<float>(b.width),
                                                 static_cast<float>(b.height)},
                                         Vector2{0.0f, 0.0f}, 0.0f, WHITE);
                    }
                });

                flecs::filter<Position, AnimatedBillboardComponent> q =
                    world.filter<Position, AnimatedBillboardComponent>();

                q.each([&](Position &p, AnimatedBillboardComponent &b) {
                    if (b.resourceHandle != NULL_HANDLE) {
                        auto texture = world.get_mut<Resources>()->textures.get(
                            b.resourceHandle);

                        Rectangle sourceRec = {
                            (float)current_frame * (float)texture.width /
                                b.numFrames,
                            0.0f, (float)texture.width / b.numFrames,
                            (float)texture.height}; // part of the texture used

                        Rectangle destRec = {
                            p.x, p.y, static_cast<float>(b.width),
                            static_cast<float>(
                                b.height)}; // where to draw texture
                        ;

                        DrawBillboardPro(debug_camera3D, texture, sourceRec,
                                         Vector3{p.x, 0.0f, p.y}, b.billUp,
                                         Vector2{static_cast<float>(b.width),
                                                 static_cast<float>(b.height)},
                                         Vector2{0.0f, 0.0f}, 0.0f, WHITE);
                    }
                });

                rlDisableBackfaceCulling();
                grass_material.maps[MATERIAL_MAP_DIFFUSE].texture =
                    grass_texture;
                static float elapsed_time = 0.0f;
                elapsed_time += iter.delta_time();
                SetShaderValue(grass_shader, loc_time, &elapsed_time,
                               SHADER_UNIFORM_FLOAT);
                int count =
                    std::min((int)grass_transforms.size(), MAX_INSTANCES);
                DrawMeshInstanced(grass_mesh, grass_material,
                                  grass_transforms.data(), count);
                rlEnableBackfaceCulling();

                // draw rocks
                flecs::filter<Position, CircleShapeRenderComponent> cirle_q =
                    world.filter<Position, CircleShapeRenderComponent>();

                cirle_q.each([&](Position &p, CircleShapeRenderComponent &s) {
                    DrawSphereWires(
                        {p.x - s.radius / 2, -s.radius / 2, p.y - s.radius / 2},
                        s.radius, 10, 10, GREEN);
                });

                auto mountain = world.get_mut<Mountain>();

                // draw player
                flecs::filter<Position, RectangleShapeRenderComponent>
                    rectangle_q =
                        world.filter<Position, RectangleShapeRenderComponent>();

                rectangle_q.each(
                    [&](Position &p, RectangleShapeRenderComponent &s) {
                        DrawCube({p.x, -0.5, p.y - s.height / 2}, s.width, 1.0,
                                 s.height, RED);
                    });

                auto killbar = world.get<KillBar>();
                DrawCube({killbar->x, 0, 0}, 20.0, 20.0, 2000.0, RED);
            }
            EndMode3D();
        }
    }
}

void renderHUD(const flecs::iter &iter) {

    // TODO get hp some other way?
    float player_health = 1; // 0.9; // percent
    flecs::filter<Health> health_q = iter.world().filter<Health>();
    health_q.each([&](Health h) { player_health = (float)h.hp / 100; });

    // health bar
    int healthbar_width = SCREEN_WIDTH / 4;
    int healthbar_height = SCREEN_HEIGHT / 30;
    DrawRectangle(20, 20, healthbar_width, healthbar_height, WHITE);
    int offset = 2;
    DrawRectangle(20 + offset, 20 + offset,
                  player_health * healthbar_width - 2 * offset,
                  healthbar_height - 2 * offset, GREEN);

    // score
    DrawText("0000", SCREEN_WIDTH * 5 / 6, 50, 40, BLACK);

    // if (GuiButton({20, 50, 140, 30}, "Button")) { //"#05#Open Image")) {
    //     alive = false;                            // TODO get from somewhere
    // }

    if (!iter.world().get_mut<AppInfo>()->playerAlive) {
        DrawText("You died!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 70, RED);
    }

    DrawFPS(0, 0);
}

void renderMenu(const flecs::iter &iter) {
    DrawText("this is a menu", SCREEN_WIDTH * 5 / 6, 50, 40, BLACK);
}

void destroy() {
    UnloadShader(grass_shader);
    UnloadTexture(gradient_texture_background);
    // UnloadModel(model);

    UnloadTexture(background_tex);
    UnloadTexture(midground_tex);
    UnloadTexture(foreground_tex);
    CloseAudioDevice();
    CloseWindow();
}

} // namespace graphics
