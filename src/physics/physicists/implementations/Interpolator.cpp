//
// Created by bjoern on 5/21/24.
//

#include "../Interpolator.hpp"

#include <iostream>
#include <mutex>

Interpolator::Interpolator() : world(World::getInstance()) {}

Interpolator::~Interpolator() = default;

void Interpolator::interpolate(floatType alpha) const {};
