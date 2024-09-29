//
// Created by bjoern on 5/21/24.
//

#include "../Positioner.hpp"

#include "spdlog/sinks/rotating_file_sink.h"

#include <iostream>
#include <mutex>

Positioner::Positioner(World &world,
                       HikerConstants &hikerConstants,
                       BarriersConstants &barriersConstants,
                       PhysicsConstants &physicsConstants,
                       TerrainConstants &terrainConstants)
    : world(world),
      deltaT(1),
      hikerConstants(hikerConstants),
      physicsConstants(physicsConstants),
      barriersConstants(barriersConstants),
      terrainConstants(terrainConstants){};

void Positioner::updatePositions() const {
    this->updateMonsterPosition();
    this->updateWorldBorderPosition();
    this->updateRockPositions();
    this->updateHikerPosition();
}

void Positioner::setDeltaT(const floatType deltaT) { this->deltaT = deltaT; };

void Positioner::updateMonsterPosition() const {
    const auto oldXPos = this->world.getMonster().getXPosition();
    const auto newXPos = oldXPos + barriersConstants.killBarVelocity * this->deltaT;
    const auto newYPos = this->world.getTerrain().getMaxHeight(newXPos);
    Vector newPos = {newXPos, newYPos};
    this->world.getMonster().setPosition(newPos);
}

void Positioner::updateWorldBorderPosition() const {
    const auto xMin = this->world.getMinX();
    const auto xMax = this->world.getMaxX();
    const auto delta = barriersConstants.killBarVelocity * this->deltaT;
    this->world.setMinX(xMin + delta);
    this->world.setMaxX(xMax + delta);
}

void Positioner::updateRockPositions() const {
    for (auto &rock : this->world.getRocks()) {
        // Symplectic euler for translation
        auto posDelta = (rock->getLinearMomentum() / rock->getMass()) * this->deltaT;
        rock->move(posDelta);

        // Symplectic Euler for rotation
        auto rotationAngleRadDelta = (rock->getAngularMomentum() / rock->getMomentOfInertia()) * this->deltaT;
        rock->rotate(rotationAngleRadDelta);
    }
}

void Positioner::moveHiker(const Vector &movement) const {
    if (movement != Vector{0.f, 0.f}) {
        Hiker &hiker = this->world.getHiker();
        Vector newPos = hiker.getPosition() + movement;
        hiker.setPosition(newPos);
        this->resolveHikerTerrainCollisions(movement);
    }
}

// void Positioner::moveHiker(Vector movement) const {
//     floatType epsilon = this->physicsConstants.epsilon;
//     Hiker &hiker = this->world.getHiker();
//     std::shared_ptr<DynamicPolygon> hikerHitBox = hiker.getCurrentBoundingBox();
//     Vector pos = hikerHitBox->getPosition();
//     hikerHitBox->move(movement);
//     std::vector<CollisionObject> terrainCollisions =
//         CollisionDetector::polygonTerrainCollision(*hikerHitBox, this->world.getTerrain());
//     bool hitTerrain = false;
//     while (!terrainCollisions.empty()) {
//         Vector retraceDirection = {0, 0};
//         floatType weightSum = 0;
//         for (const CollisionObject &collisionObject : terrainCollisions) {
//             retraceDirection += collisionObject.collisionDirection * collisionObject.collisionDepth;
//             weightSum += collisionObject.collisionDepth;
//         }
//         retraceDirection /= weightSum;
//         Vector retracedMovement =
//             retraceDirection * (weightSum / static_cast<floatType>(terrainCollisions.size()) + epsilon);
//         hikerHitBox->move(retracedMovement);
//         terrainCollisions = CollisionDetector::polygonTerrainCollision(*hikerHitBox, this->world.getTerrain());
//         hitTerrain = true;
//     }
//     Vector newPos = hiker.getPosition() + (hikerHitBox->getPosition() - pos);
//     if (hitTerrain) {
//         hiker.setYVelocity(0.f);
//     }
//     hiker.setPosition(newPos);
// };

void Positioner::applyKnockbackToHiker() const {
    if (this->world.getHiker().getKnockback().length() < NUMERIC_EPSILON) {
        this->world.getHiker().setKnockback({0, 0});
        return;
    }
    const Vector &knockback = this->world.getHiker().getKnockback();
    this->moveHiker(knockback * this->deltaT);
    this->world.getHiker().setKnockback(knockback * this->hikerConstants.knockbackLossPerStep);
    /*const Vector &knockback = this->world.getHiker().getKnockback();
    const bool shouldBeKnockedAgain = knockback.length() > this->hikerConstants.knockbackLossPerStep;
    if (shouldBeKnockedAgain) {
        this->moveHiker(knockback * this->deltaT);
        this->world.getHiker().setKnockback(knockback - (knockback * this->hikerConstants.knockbackLossPerStep));
    } else {
        this->world.getHiker().setKnockback({0, 0});
    }*/
}

void Positioner::clampHikerToTerrain() const {
    Hiker &hiker = this->world.getHiker();
    Vector hikerFeetPos = hiker.getPosition();
    floatType terrainHeight = this->world.getTerrain().mapHeightToTerrain(hikerFeetPos);
    hikerFeetPos.y = terrainHeight;
    hiker.setPosition(hikerFeetPos);
    hiker.setHikerMoving();
    hiker.setLastJump(0.0f);
}

void Positioner::moveHikerInAir() const {
    Hiker &hiker = this->world.getHiker();
    const auto vel = hiker.getVelocity();
    Vector movement = {vel.x * this->deltaT *
                           (this->hikerConstants.normalSpeed + this->hikerConstants.airMovementSpeedFactor),
                       vel.y * this->deltaT};
    this->moveHiker(movement);
    this->updateHikerState(movement);
}

void Positioner::moveHikerOnTerrain() const {
    Hiker &hiker = this->world.getHiker();
    Vector oldPos = hiker.getPosition();
    Vector newPos = this->calculateTheoreticalNextHikerPosition();
    Vector direction = newPos - oldPos;

    if (direction.x == 0 || direction.computeSlope() > this->hikerConstants.maxClimbableSlope) {
        if (direction.y < 0) {
            spdlog::debug("falling off terrain");
            // Fall off terrain
            this->moveHiker({direction.x, 0.0f});
            hiker.setHikerInAir();
            hiker.setLastJump(0.0);
        } else if (direction.y > 0) {
            // TODO: Slide downhill
            hiker.accelerateY((this->physicsConstants.gravitationalConstant + this->hikerConstants.friction) *
                              this->deltaT);
            hiker.setXVelocity(0.0f);
            this->moveHiker(hiker.getVelocity() * this->deltaT);
            this->updateHikerState(hiker.getPosition() - oldPos);
        }
    } else {
        spdlog::debug("moving along terrain");
        this->moveHiker({direction.x, direction.y + this->physicsConstants.epsilon});
        this->clampHikerToTerrain();
    }
}

void Positioner::enforceWorldLimitOnHiker() const {
    Hiker &hiker = this->world.getHiker();
    Vector pos = hiker.getPosition();

    if (pos.x > this->world.getMonster().getXPosition() + barriersConstants.playerRightBarrierOffset) {
        pos.x = this->world.getMonster().getXPosition() + barriersConstants.playerRightBarrierOffset;
        if (hiker.getHikerMovement().getState() != HikerMovement::MovementState::IN_AIR) {
            pos.y = this->world.getTerrain().mapHeightToTerrain(pos);
        }
    }
    hiker.setPosition(pos);
}

void Positioner::updateHikerPosition() const {

    this->applyKnockbackToHiker();
    Hiker &hiker = this->world.getHiker();
    if (hiker.getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
        this->moveHikerInAir();
    } else {
        this->moveHikerOnTerrain();
    }
    this->enforceWorldLimitOnHiker();
}

// void Positioner::updateHikerPosition() const { // NOLINT(*-function-size)
//     floatType knockback = 0;
//     Hiker &hiker = this->world.getHiker();
//     // TODO this should go into the collision handler, but there might be problems with the event processor so let's
//     // TODO leave it here until we take care of the physics
//     if (hiker.getIsHit()) {
//         const floatType radius = hiker.getHitInformation().radiusRock;
//         const floatType rockVelocity = hiker.getHitInformation().velocityRock;
//         // TODO KNOCKBACK could vary with rock type, e.g. small rocks with little damage but massive knockback
//         knockback = rockVelocity * hikerConstants.knockBack * radius;
//         int counter = hiker.getHitInformation().countingVariable;
//         counter++;
//         // TODO I don't like the HitInformation
//         hiker.setHitInformation({radius, rockVelocity, counter});
//         // TODO this 50 feels like a poor implementation choice and should at least be some sort of constant
//         // TODO like KNOCKBACK_TIME
//         // TODO or, we should do it via forces and impulses so we don't need that at all, i.e. the player input is a
//         // TODO force not a set velocity
//         if (counter >= 50) {
//             hiker.setIsHit(false);
//             hiker.setHitInformation({0.0f, 0.0f, 0});
//         }
//     }
//
//     auto pos = hiker.getPosition();
//     const auto vel = hiker.getVelocity();
//     Vector knockbackVector = {0, 0}; //{knockback, 0.0f};
//     if (hiker.getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
//         spdlog::debug("in air");
//         // check collision lower point of hiker
//         floatType nextXPos =
//             pos.x +
//             vel.x * this->deltaT * (this->hikerConstants.normalSpeed + this->hikerConstants.airMovementSpeedFactor) +
//             knockbackVector.x;
//         floatType nextYPos = pos.y + vel.y * this->deltaT + knockbackVector.y;
//         Vector newPos = {nextXPos, nextYPos};
//         Line intersectionLine = {pos, newPos};
//         std::vector<std::shared_ptr<Intersection>> intersections =
//             this->world.getTerrain().getAllIntersections(intersectionLine);
//         // check collision upper point of hiker
//         Vector heightDelta = {0.0f, hikerConstants.hikerHeight};
//         Vector upperHikerBound = pos + heightDelta;
//         Vector newUpperPos = newPos + heightDelta;
//         Line upperIntersectionLine = {upperHikerBound, newUpperPos};
//         std::vector<std::shared_ptr<Intersection>> upperHikerBoundIntersections =
//             this->world.getTerrain().getAllIntersections(upperIntersectionLine);
//
//         if (!intersections.empty() && intersections.front()->intersection != pos) {
//             pos = intersections.front()->intersection;
//             hiker.setYVelocity(0.f);
//             hiker.setHikerMoving();
//             hiker.setLastJump(0.0);
//             // } else if (!upperHikerBoundIntersections.empty() && intersections.front()->intersection != pos +
//             // heightDelta) {
//             //    pos = {upperHikerBoundIntersections.front()->intersection.x,
//             //           upperHikerBoundIntersections.front()->intersection.y - hikerConstants.hikerHeight};
//             //    hiker.setYVelocity(-0.01f);
//         } else {
//             pos = newPos;
//         }
//     } else {
//         Vector newPos = this->calculateTheoreticalNextHikerPosition(knockbackVector);
//         Vector direction = newPos - pos;
//
//         if (direction.x == 0 || direction.computeSlope() > this->hikerConstants.maxClimbableSlope) {
//             if (direction.y < 0) {
//                 spdlog::debug("falling off terrain");
//                 // Fall off terrain
//                 pos.x = newPos.x;
//                 hiker.setHikerInAir();
//                 hiker.setLastJump(0.0);
//             } else {
//                 spdlog::debug("standing still");
//                 // Stand still
//                 // TODO: Slide downhill
//             }
//         } else {
//             spdlog::debug("moving along terrain");
//             pos = newPos;
//         }
//     }
//     if (pos.x > this->world.getMonster().getXPosition() + barriersConstants.playerRightBarrierOffset) {
//         pos.x = this->world.getMonster().getXPosition() + barriersConstants.playerRightBarrierOffset;
//         pos.y = this->world.getTerrain().mapHeightToTerrain(pos);
//     }
//     hiker.setPosition(pos);
// }

Vector Positioner::calculateTheoreticalNextHikerPosition() const {
    Hiker &hiker = this->world.getHiker();
    const auto vel = hiker.getVelocity();
    const auto oldPos = hiker.getPosition();
    floatType nextXPos;
    floatType nextYPos;
    spdlog::debug("Hiker direction: {}", hiker.getHikerMovement().getDirection());
    if (hiker.getHikerMovement().getDirection() != HikerMovement::Direction::NEUTRAL) {
        nextXPos = vel.x * this->deltaT * this->hikerConstants.maxSpeedNegSlope + oldPos.x;
        nextYPos = this->world.getTerrain().mapHeightToTerrain({nextXPos, oldPos.y});
        Vector newPos = {nextXPos, nextYPos};

        auto direction = newPos - oldPos;
        const floatType speedFactor = this->world.getHiker().computeSpeedFactor(direction);
        nextXPos = oldPos.x + this->deltaT * vel.x * speedFactor;
        nextYPos = this->world.getTerrain().mapHeightToTerrain({nextXPos, oldPos.y});
    } else {
        nextXPos = oldPos.x;
        nextYPos = oldPos.y;
    }
    Vector newPos = {nextXPos, nextYPos};
    return newPos;
}

void Positioner::resolveHikerTerrainCollisions(Vector movement) const {
    Hiker &hiker = this->world.getHiker();
    std::shared_ptr<StaticPolygon> hikerHitBox = hiker.getCurrentBoundingBoxStatic();
    std::vector<std::shared_ptr<Intersection>> terrainCollisions =
        this->world.getTerrain().calculateCollisionsWithPolygon(hikerHitBox);
    bool hitTerrain = false;
    int counter = 0;
    Vector lastNormal{};
    while (!terrainCollisions.empty()) {
        Vector retraceDirection = {0, 0};
        for (const auto &intersection : terrainCollisions) {
            retraceDirection += intersection->normalAtIntersection;
        }
        retraceDirection /= static_cast<floatType>(terrainCollisions.size());
        Vector newPosition = hiker.getPosition() + retraceDirection * movement.length();
        hiker.setPosition(newPosition);
        terrainCollisions = this->world.getTerrain().calculateCollisionsWithPolygon(hikerHitBox);
        hikerHitBox = hiker.getCurrentBoundingBoxStatic();
        hitTerrain = true;
        counter++;
        lastNormal = retraceDirection.normalize();
    }
    if (hitTerrain) {
        Vector oldVelocity = hiker.getVelocity();
        Vector newVelocity =
            oldVelocity.reflectOnNormalDampened(lastNormal, this->hikerConstants.terrainCollisionDampening);
        hiker.setVelocity(newVelocity);
    }
}

bool Positioner::hikerFeetNearTerrain(Vector movement) const {
    Hiker &hiker = this->world.getHiker();
    Vector hikerFeetPos = hiker.getPosition();
    floatType terrainHeight = this->world.getTerrain().mapHeightToTerrain(hikerFeetPos);
    if (fabs(hikerFeetPos.y - terrainHeight) < fabs(movement.y) && movement.y < 0) {
        return true;
    } else {
        return false;
    }
}

void Positioner::updateHikerState(Vector movement) const {
    Hiker &hiker = this->world.getHiker();
    if (this->hikerFeetNearTerrain(movement)) {
        hiker.setHikerMoving();
        hiker.setVelocity({0.0f, 0.0f});
        hiker.setLastJump(0.0f);
        this->clampHikerToTerrain();
    } else {
        hiker.setHikerInAir();
    }
}
