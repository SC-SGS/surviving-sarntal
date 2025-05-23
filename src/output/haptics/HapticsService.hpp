//
// Created by felix-roehr on 6/8/24.
//

#ifndef SURVIVING_SARNTAL_HAPTICSSERVICE_HPP
#define SURVIVING_SARNTAL_HAPTICSSERVICE_HPP

#include "../../input/InputHandler.h"
#include "../../utilities/Singleton.hpp"
#include <SDL2/SDL.h>

class HapticsService : public Singleton<HapticsService> {
    friend class Singleton<HapticsService>;

  public:
    /**
     * If there is a gamepad connected, provide haptic feedback.
     *
     * @param strength
     * @param duration
     */
    static void rumble(int strength, int duration, InputHandler &inputHandler);

    /**
     * Executes appropriate rumble for the given rockDmg.
     *
     * @param rockDmg
     */
    static void rockRumble(int rockDmg, InputHandler &inputHandler);

    /**
     * Execute appropriate rumble for when the hiker dies.
     */
    static void deathRumble(InputHandler &inputHandler);

  private:
    HapticsService();
    ~HapticsService();

    static void rumbleThread(SDL_GameController *controller, int strength, int duration);
};

#endif // SURVIVING_SARNTAL_HAPTICSSERVICE_HPP
