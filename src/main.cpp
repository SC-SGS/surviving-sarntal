#include "components/mountain.h"
#include "components/particle_state.h"
#include "raylib.h"
#include "systems/game_logic.h"
#include "systems/input_systems.h"
#include "systems/physics.h"
#include "systems/render_systems.h"
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

int main() {
    using namespace std::chrono_literals;
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;
    kinect_mode = false;
    kinect_init = false;

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

    world.set<Mountain>({});
    graphics::initRenderSystem(world);
    graphics::prepareGameResources(world);
    graphics::prepareMenuResources(world);

    initGameLogic(world);

    physics::initSounds();

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
