//
// Created by bjoern on 5/21/24.
//

#include "Interpolator.hpp"

#include <iostream>
#include <mutex>

Interpolator::Interpolator() : world(World::getInstance()) {
    std::cout << "Interpolator gets constructed" << std::endl;
}

Interpolator::~Interpolator() { std::cout << "Interpolator gets deconstructed" << std::endl; }

void Interpolator::interpolate(float alpha) const {};
