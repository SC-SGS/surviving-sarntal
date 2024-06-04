//
// Created by bjoern on 5/21/24.
//

#include "Accelerator.hpp"

#include <iostream>
#include <mutex>
// TODO #include <omp.h>

Accelerator::Accelerator() : world(World::getInstance()), deltaT(1) {
    std::cout << "Accelerator gets constructed" << std::endl;
};
Accelerator::~Accelerator() { std::cout << "Accelerator gets deconstructed" << std::endl; }

void Accelerator::accelerate() const {
    this->updateHikerVelocity();
    this->updateRockVelocities();
    this->updateHikerDirection();
}
void Accelerator::setDeltaT(float deltaT) { this->deltaT = deltaT; }

void Accelerator::updateRockVelocities() const {
    // TODO #pragma omp parallel for
    for (auto &rock : this->world.getRocks()) {
        auto vel = rock.getVelocity();
        vel.y += GRAVITATIONAL_CONSTANT * this->deltaT;
        // TODO rock speed probably shouldn't be capped
        if (vel.length() > VELOCITY_CAP) {
            vel = (Velocity)(vel * VELOCITY_CAP / vel.length());
        }
        rock.setVelocity(vel);
    }
}

void Accelerator::updateHikerVelocity() const {
    if (this->world.getHiker().getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
        this->world.getHiker().getHikerMovement().setLastJump(this->world.getHiker().getHikerMovement().getLastJump() +
                                                              this->deltaT);
        auto vel = this->world.getHiker().getVelocity();
        vel.y += GRAVITATIONAL_CONSTANT * this->deltaT;
        this->world.getHiker().setVelocity(vel);
    }
}

void Accelerator::updateHikerDirection() const {
    auto &hiker = this->world.getHiker();
    auto vel = hiker.getVelocity();
    if (vel.x < 0) {
        hiker.getHikerMovement().setDirection(HikerMovement::Direction::LEFT);
    } else if (vel.x > 0) {
        hiker.getHikerMovement().setDirection(HikerMovement::Direction::RIGHT);
    } else {
        hiker.getHikerMovement().setDirection(HikerMovement::Direction::NEUTRAL);
    }
    // std::cout << "Updated hiker direction: " << this->world.getHiker().getHikerMovement().getDirection() <<
    // std::endl;
};