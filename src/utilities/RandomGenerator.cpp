//
// Created by Aleksis Vezenkov on 08.06.24.
//

#include "RandomGenerator.hpp"

int RandomGenerator::getRandomNumber(const int min, const int max) {
    std::uniform_int_distribution<> distribution(min, max);
    return distribution(gen);
}

floatType RandomGenerator::getRandomRealNumber(const float min, const float max) {
    std::uniform_real_distribution<> distribution(min, max);
    return static_cast<floatType>(distribution(gen));
}
