//
// Created by Aleksis Vezenkov on 08.06.24.
//

#include "RandomGenerator.hpp"

int RandomGenerator::getRandomNumber(int min, int max) {
    std::uniform_int_distribution<> distribution(min, max);
    return distribution(gen);
}

floatType RandomGenerator::getRandomRealNumber(float min, float max) {
    std::uniform_real_distribution<> distribution(min, max);
    return static_cast<floatType>(distribution(gen));
}
