#include "components/mountain.h"
#include "components/particle_state.h"
#include "raylib.h"
#include "systems/game_logic.h"
#include "systems/input_systems.h"
#include "systems/physics.h"
#include "systems/render_systems.h"
#include "utils/game_constants.h"
#include <chrono>
#include <flecs.h>
#include <iostream>

#ifdef kinect
#include "components/kinect_handler.h"
#include <thread>
#endif

void mainLoop(flecs::world &world);

bool kinect_mode;

int main() {
    using namespace std::chrono_literals;
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;
    kinect_mode = false;

#ifdef kinect
    std::cout << "Kinect is active" << std::endl;
    kinect_mode = true;
    std::thread kinect_thread(initKinect);
    std::this_thread::sleep_for(10s);
#endif

    world.import <PhysicSystems>();
    world.import <InputSystems>();

    world.set<Mountain>({});
    graphics::initRenderSystem(world);
    graphics::prepareGame(world);
    graphics::prepareHUD(world);
    graphics::prepareMenu(world);

    initGameLogic(world);

    world.set<AppInfo>({});

    mainLoop(world);

    graphics::destroy();
}

void mainLoop(flecs::world &world) {
    world.progress(0);

    SetTargetFPS(1000);

    auto app_info = world.get<AppInfo>();

    while (app_info->isRunning) {
        float dt = GetFrameTime();
        world.progress(dt);
    }
}
