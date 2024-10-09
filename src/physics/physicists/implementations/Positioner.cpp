//
// Created by bjoern on 5/21/24.
//

#include "../Positioner.hpp"

#include "../CollisionDetector.hpp"
#include "spdlog/sinks/rotating_file_sink.h"

#include <iostream>

Positioner::Positioner(World &world,
                       HikerConstants &hikerConstants,
                       BarriersConstants &barriersConstants,
                       PhysicsConstants &physicsConstants,
                       TerrainConstants &terrainConstants)
    : world(world),
      deltaT(1),
      hikerConstants(hikerConstants),
      barriersConstants(barriersConstants),
      terrainConstants(terrainConstants),
      physicsConstants(physicsConstants) {}

void Positioner::updatePositions() const {
    this->updateMonsterPosition();
    this->updateWorldBorderPosition();
    this->updateRockPositions();
    this->updateHikerPosition();
}

void Positioner::setDeltaT(const floatType deltaT) { this->deltaT = deltaT; }

void Positioner::updateMonsterPosition() const {
    const auto oldXPos = this->world.getMonster().getXPosition();
    const auto offset = this->world.getMonster().getVelocity() * this->deltaT;
    const auto newXPos = oldXPos + offset;
    const auto newYPos = this->world.getTerrain().getMaxHeight(newXPos);
    const Vector newPos = {newXPos, newYPos};
    this->world.getMonster().setPosition(newPos);
}

void Positioner::updateWorldBorderPosition() const {
    const auto xMin = this->world.getMinX();
    const auto xMax = this->world.getMaxX();
    const auto delta = this->world.getMonster().getVelocity() * this->deltaT;
    this->world.setMinX(xMin + delta);
    this->world.setMaxX(xMax + delta);
}

void Positioner::updateRockPositions() const {
    for (const auto &rock : this->world.getRocks()) {
        // Symplectic euler for translation
        auto posDelta = (rock->getLinearMomentum() / rock->getMass()) * this->deltaT;
        rock->move(posDelta);

        // Symplectic Euler for rotation
        const auto rotationAngleRadDelta = (rock->getAngularMomentum() / rock->getMomentOfInertia()) * this->deltaT;
        rock->rotate(rotationAngleRadDelta);
    }
}

void Positioner::applyKnockbackToHiker() const {
    if (this->world.getHiker().getKnockback().length() < this->hikerConstants.knockbackCutoff) {
        this->world.getHiker().setKnockback({0, 0});
        return;
    }
    const Vector &knockback = this->world.getHiker().getKnockback();
    this->world.getHiker().setVelocity(this->world.getHiker().getVelocity() + knockback);
    this->world.getHiker().setKnockback(knockback * this->hikerConstants.knockbackLossPerStep);
}

void Positioner::moveHikerOnTerrain() const {
    Hiker &hiker = this->world.getHiker();
    const Vector lastMovement =
        hiker.getCurrentHitbox()->getPosition() - hiker.getCurrentHitbox()->getDynamicProperties().lastPosition;
    floatType speedFactor;
    if (hiker.getVelocity().x * lastMovement.x > 0) {
        // Moving in the same direction as before
        speedFactor = hiker.computeSpeedFactor(lastMovement);
    } else {
        // Changed direction
        speedFactor = 1.f;
    }
    if (speedFactor <= NUMERIC_EPSILON) {
        spdlog::debug("Can't move");
        hiker.move({0.f, 0.f});
    }
    const Vector movement = this->calculateTheoreticalNextHikerPosition(speedFactor) - hiker.getPosition();
    if (movement.y < 0 && movement.x != 0 && movement.computeSlope() > this->hikerConstants.maxClimbableSlope) {
        spdlog::debug("falling off terrain");
        hiker.move(
            hiker.getVelocity() * this->deltaT * this->hikerConstants.maxSpeedNegSlope +
            Vector{physicsConstants.epsilon * static_cast<floatType>(hiker.getHikerMovement().getDirection()), 0});
        hiker.setInAir();
    } else {
        if (movement.x != 0) {
            hiker.move(movement);
        }
    }
}

void Positioner::updateHikerPosition() const {
    this->applyKnockbackToHiker();
    Hiker &hiker = this->world.getHiker();
    if (hiker.getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
        hiker.move(hiker.getVelocity() * this->deltaT);
        // this->moveHikerInAir();
    } else {
        // hiker.move(hiker.getVelocity() * this->deltaT);
        this->moveHikerOnTerrain();
    }
}

Vector Positioner::calculateTheoreticalNextHikerPosition(const floatType speedFactor) const {
    const Hiker &hiker = this->world.getHiker();
    const auto vel = hiker.getVelocity();
    const auto oldPos = hiker.getPosition();
    spdlog::debug("Hiker direction: {}", hiker.getHikerMovement().getDirection());
    Vector newPos = oldPos + vel * speedFactor * this->deltaT;
    newPos.y = this->world.getTerrain().mapToClosestTopTerrain(newPos) + this->physicsConstants.epsilon;
    return newPos;
}
