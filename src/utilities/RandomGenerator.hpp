//
// Created by Aleksis Vezenkov on 08.06.24.
//

#ifndef SURVIVING_SARNTAL_RANDOMGENERATOR_HPP
#define SURVIVING_SARNTAL_RANDOMGENERATOR_HPP

#include "../game/GameProperties.hpp"
#include "Singleton.hpp"
#include <iostream>
#include <random>

class RandomGenerator : public Singleton<RandomGenerator> {
    friend class Singleton<RandomGenerator>;

  public:
    RandomGenerator() : gen(rd()) {}

    int getRandomNumber(int min, int max);

    floatType getRandomRealNumber(float min, float max);

  private:
    std::random_device rd;
    std::mt19937 gen;
};

#endif // SURVIVING_SARNTAL_RANDOMGENERATOR_HPP
