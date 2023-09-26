#include "components/mountain.h"
#include "components/particle_state.h"
#include "raylib.h"
#include "systems/game_logic.h"
#include "systems/input_systems.h"
#include "systems/physics.h"
#include "systems/render_systems.h"
#include <flecs.h>
#include <iostream>

#ifdef kinect
#include <XnOpenNI.h>
#include <XnVWaveDetector.h>
#include <cstdio>

// xml to initialize OpenNI
#define SAMPLE_XML_FILE "../../../Data/Sample-Tracking.xml"
#define SAMPLE_XML_FILE_LOCAL "Sample-Tracking.xml"

// callback for a new position of any hand
void XN_CALLBACK_TYPE onPointUpdate(const XnVHandPointContext *pContext,
                                    void *cxt) {
    printf("%d: (%f,%f,%f) [%f]\n", pContext->nID, pContext->ptPosition.X,
           pContext->ptPosition.Y, pContext->ptPosition.Z, pContext->fTime);
}

#endif

void mainLoop(flecs::world &world);

int main() {
    std::cout << "surviving sarntal" << std::endl;

    flecs::world world;

#ifdef kinect
    std::cout << "Kinect is active" << std::endl;
#endif

    world.import <PhysicSystems>();
    world.import <InputSystems>();

    world.set<Mountain>({});
    graphics::init_render_system(world);

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
