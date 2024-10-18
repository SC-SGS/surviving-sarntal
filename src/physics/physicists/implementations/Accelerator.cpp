//
// Created by bjoern on 5/21/24.
//

#include "../Accelerator.hpp"
#include <iostream>

Accelerator::Accelerator(World &world, GameConstants &gameConstants)
    : gameConstants(gameConstants), world(world), deltaT(1) {}

void Accelerator::accelerate() const {
    this->updateHikerVelocity();
    this->updateMomentumOfRocks();
    this->updateMonsterVelocity();
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
        sumOfTorque += vertex.cross(forcePerVertex * this->gameConstants.physicsConstants.gravitySpinEffectOnVertices);
    }
    return sumOfTorque;
}

void Accelerator::updateMomentumOfRocks() const {
    // TODO parallel batch processing
    for (auto &rock : this->world.getRocks()) {
        auto linearMomentum = rock->getLinearMomentum();
        linearMomentum += this->calculateTranslationalForces(*rock) * this->deltaT;
        rock->setLinearMomentum(linearMomentum);
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

void Accelerator::updateMonsterVelocity() const {
    floatType factor = this->world.getMaxX() * gameConstants.barriersConstants.killBarAccelerationFactor;
    factor = fminf(factor, gameConstants.barriersConstants.maxKillBarFactor);
    this->world.getMonster().setVelocity(gameConstants.barriersConstants.killBarBaseVelocity * factor);
}