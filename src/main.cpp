#include "components/inventory.h"
#include "components/mountain.h"
#include "components/particle_state.h"
#include "raylib.h"
#include "systems/game_logic.h"
#include "systems/input_systems.h"
#include "systems/physics.h"
#include "systems/render_systems.h"
#include "utils/game_constants.h"
#include "utils/kinect_variables.h"
#include <chrono>
#include <flecs.h>
#include <iostream>

#ifdef kinect
#include "components/kinect_handler.h"
#include <thread>
#endif

void mainLoop(flecs::world &world);

bool kinect_mode;

bool kinect_init;

int main(int argc, char *argv[]) {
    using namespace std::chrono_literals;
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;

    world.set<InputEntity>(InputEntity{});
    kinect_mode = false;
    kinect_init = false;

    constexpr std::hash<std::string> hasher{};

    auto hash = hasher("path");
    auto hash2 = hasher("path");

#ifdef kinect
    std::cout << "Kinect is active" << std::endl;
    kinect_mode = true;
    std::thread kinect_thread(initKinect);
    while (!kinect_init) {
        std::cout << "Waiting for kinect synchronization" << std::endl;
        std::this_thread::sleep_for(5s);
    }

#endif

    world.import <PhysicSystems>();
    world.import <InputSystems>();

    world.set<Inventory>({7});
    world.set<Mountain>({});

    graphics::initStartScreen(world);

    while (!IsKeyPressed(KEY_ENTER)) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawText("Surviving Sarntal", 500, 350, 70, WHITE);
            DrawText("Press Enter to start", 450, 500, 70, WHITE);
        }
        EndDrawing();
    }
    graphics::initRenderSystem(world);
    graphics::prepareGameResources(world);
    graphics::prepareMenuResources(world);

    initGameLogic(world);

    bool meme_mode = false;
    if (argc >= 2) {
        std::string music_mode = argv[1];
        if (music_mode == "meme") {
            meme_mode = true;
        }
    }
    physics::initSounds(meme_mode);

    world.set<AppInfo>({});

    mainLoop(world);

    graphics::destroy();
}

void mainLoop(flecs::world &world) {
    world.progress(0);

    SetTargetFPS(1000);

    auto app_info = world.get<AppInfo>();
    bool is_Paused = false;

    while (app_info->isRunning) {
        if (app_info->playerAlive) {
            if (world.get<InputEntity>()->getEvent(Event::PAUSE)) {
                is_Paused = !is_Paused;
            }
            if (!is_Paused) {
                std::cout << "HELLOOO" << std::endl;
                float dt = GetFrameTime();
                std::cout << "Frame Rate:" << 1 / dt << std::endl;
                world.progress(dt);
            } else {
                BeginDrawing();
                {
                    ClearBackground(WHITE);
                    DrawText("Game Paused", 600, 450, 70, BLACK);
                    DrawText("Press Backspace to continue!!", 350, 550, 70,
                             BLACK);
                }
                EndDrawing();
            }
        } else {
        }
    }
}
