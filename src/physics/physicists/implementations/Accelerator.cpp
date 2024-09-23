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
    this->updateMomentumOfRocks();
    this->updateHikerDirection();
}
void Accelerator::setDeltaT(const floatType deltaT) { this->deltaT = deltaT; }

Vector Accelerator::calculateTranslationalForces(const Rock &rock) const {
    Vector sumOfForces{};
    sumOfForces += Vector{0, 1} * this->gameConstants.physicsConstants.gravitationalConstant * rock.getMass();
    return sumOfForces;
}

floatType Accelerator::calculateTorque(const Rock &rock) const {
    floatType sumOfTorque = 0.0f;
    // Proper fall
    const Vector forcePerVertex =
        Vector{0, this->gameConstants.physicsConstants.gravitationalConstant * rock.getMass()} /
        static_cast<floatType>(rock.getBodySpaceVertices().size());
    for (const auto &vertex : rock.getBodySpaceVertices()) {
        sumOfTorque += vertex.cross(forcePerVertex);
    }
    return sumOfTorque;
}

void Accelerator::updateMomentumOfRocks() const {
    // TODO #pragma omp parallel for
    for (auto &rock : this->world.getRocks()) {
        auto linearMomentum = rock->getLinearMomentum();
        linearMomentum += this->calculateTranslationalForces(*rock) * this->deltaT;
        rock->setLinearMomentum(linearMomentum);
        // TODO check if velocity / momentum neeeds to be capped

        auto angularMomentum = rock->getAngularMomentum();
        angularMomentum += this->calculateTorque(*rock) * this->deltaT;
        rock->setAngularMomentum(angularMomentum);
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