//
// Created by felix-roehr on 6/8/24.
//

#include "HapticsService.hpp"
#include "../../input/InputHandler.h"
#include "../../input/devices/Gamepad.h"
#include <iostream>
#include <thread>

HapticsService::HapticsService() = default;

HapticsService::~HapticsService() { SDL_Quit(); }

void HapticsService::rumble(int strength, int duration, InputHandler &inputHandler) {
    for (auto device : inputHandler.getDevices()) {
        if (device->getDevice() == DEVICE_GAMEPAD) {
            auto controller = SDL_GameControllerOpen(device->getId());
            spdlog::debug("Is Controller {} an SDL controller? {}", device->getId(), static_cast<bool>(controller));
            if (controller) {
                //  Create a thread to run the rumble effect
                std::thread rumbleThreadObj(HapticsService::rumbleThread, controller, strength, duration);

                // Detach the thread to allow it to run independently
                rumbleThreadObj.detach();
            }
        }
    }
}

void HapticsService::rockRumble(int rockDmg, InputHandler &inputHandler) {
    HapticsService::rumble(std::min(rockDmg * 4000, 65535), 300, inputHandler);
}

void HapticsService::deathRumble(InputHandler &inputHandler) { HapticsService::rumble(65535, 3000, inputHandler); }

void HapticsService::rumbleThread(SDL_GameController *controller, const int strength, const int duration) {
    // Set rumble intensity for both motors (left and right)
    SDL_GameControllerRumble(controller, strength, strength,
                             duration); // Values range from 0 to 65535

    // Sleep for the specified duration
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));

    // Stop the rumble effect
    SDL_GameControllerRumble(controller, 0, 0, 0);
}