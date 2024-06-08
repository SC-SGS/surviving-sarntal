//
// Created by Aleksis Vezenkov on 08.06.24.
//

#include "RandomGenerator.hpp"
int RandomGenerator::getRandomNumber(int min, int max) {
    std::uniform_int_distribution<> distribution(min, max);
    return distribution(gen);
}
