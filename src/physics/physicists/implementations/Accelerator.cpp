//
// Created by bjoern on 5/21/24.
//

#include "../Accelerator.hpp"

#include <iostream>
#include <mutex>
// TODO #include <omp.h>

Accelerator::Accelerator(World &world, GameConstants &gameConstants)
    : world(world), gameConstants(gameConstants), deltaT(1){};

void Accelerator::accelerate() const {
    this->updateHikerVelocity();
    this->updateRockVelocities();
    this->updateHikerDirection();
}
void Accelerator::setDeltaT(const floatType deltaT) { this->deltaT = deltaT; }

void Accelerator::updateRockVelocities() const {
    // TODO #pragma omp parallel for
    for (auto &rock : this->world.getRocks()) {
        auto vel = rock->getLinearMomentum();
        vel.y += gameConstants.physicsConstants.gravitationalConstant * this->deltaT;
        // TODO rock speed probably shouldn't be capped
        if (vel.length() > gameConstants.rockConstants.velocityCap) {
            vel = (Vector)(vel * gameConstants.rockConstants.velocityCap / vel.length());
        }
        rock->setLinearMomentum(vel);
    }
}

void Accelerator::updateHikerVelocity() const {
    Hiker &hiker = this->world.getHiker();
    if (hiker.getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
        hiker.setLastJump(this->world.getHiker().getHikerMovement().getLastJump() + this->deltaT);
        hiker.accelerateY(gameConstants.physicsConstants.gravitationalConstant * this->deltaT);
    }
}

void Accelerator::updateHikerDirection() const {
    auto &hiker = this->world.getHiker();
    auto vel = hiker.getVelocity();
    if (vel.x < 0) {
        hiker.turnLeft();
    } else if (vel.x > 0) {
        hiker.turnRight();
    } else {
        hiker.turnNeutral();
    }
    // std::cout << "Updated hiker direction: " << this->world.getHiker().getHikerMovement().getDirection() <<
    // std::endl;
};