//
// Created by six on 6/2/24.
//

#ifndef SURVIVING_SARNTAL_ANIMATIONINFORMATION_H
#define SURVIVING_SARNTAL_ANIMATIONINFORMATION_H

#include "../../../utilities/GameConstants.hpp"

struct AnimationInformation {
    int frames;
    int currentFrame;
    floatType frameTime;
    floatType lastTime;
};

#endif // SURVIVING_SARNTAL_ANIMATIONINFORMATION_H
