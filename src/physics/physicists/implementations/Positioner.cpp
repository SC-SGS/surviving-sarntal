//
// Created by bjoern on 5/21/24.
//

#include "../Positioner.hpp"

#include <iostream>
#include <mutex>

Positioner::Positioner(World &world, HikerConstants hikerConstants, BarriersConstants barriersConstants)
    : world(world), hikerConstants(hikerConstants), barriersConstants(barriersConstants), deltaT(1){};

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
        auto pos = rock.getPosition();
        pos += rock.getLinearMomentum() * this->deltaT;
        rock.setPosition(pos);
        const floatType newAngularOffset = rock.getRotationAngle() + rock.getAngularMomentum() * this->deltaT;
        rock.setRotationAngleRad(newAngularOffset);
    }
}

void Positioner::updateHikerPosition() const { // NOLINT(*-function-size)
    floatType knockback = 0;
    Hiker &hiker = this->world.getHiker();
    // TODO this should go into the collision handler, but there might be problems with the event processor so let's
    // TODO leave it here until we take care of the physics
    if (hiker.getIsHit()) {
        const floatType radius = hiker.getHitInformation().radiusRock;
        const floatType rockVelocity = hiker.getHitInformation().velocityRock;
        // TODO KNOCKBACK could vary with rock type, e.g. small rocks with little damage but massive knockback
        knockback = rockVelocity * hikerConstants.knockBack * radius;
        int counter = hiker.getHitInformation().countingVariable;
        counter++;
        // TODO I don't like the HitInformation
        hiker.setHitInformation({radius, rockVelocity, counter});
        // TODO this 50 feels like a poor implementation choice and should at least be some sort of constant
        // TODO like KNOCKBACK_TIME
        // TODO or, we should do it via forces and impulses so we don't need that at all, i.e. the player input is a
        // TODO force not a set velocity
        if (counter >= 50) {
            hiker.setIsHit(false);
            hiker.setHitInformation({0.0f, 0.0f, 0});
        }
    }
    auto pos = hiker.getPosition();
    const auto vel = hiker.getVelocity();
    Vector knockbackVector = {knockback, 0.0f};
    if (hiker.getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
        spdlog::debug("in air");
        // check collision lower point of hiker
        Vector newPos = pos + vel * this->deltaT + knockbackVector;
        Line intersectionLine = {pos, newPos};
        std::vector<std::shared_ptr<Intersection>> intersections =
            this->world.getTerrain().getAllIntersections(intersectionLine);
        // check collision upper point of hiker
        Vector heightDelta = {0.0f, hikerConstants.hikerHeight};
        Vector upperHikerBound = pos + heightDelta;
        Vector newUpperPos = newPos + heightDelta;
        Line upperIntersectionLine = {upperHikerBound, newUpperPos};
        std::vector<std::shared_ptr<Intersection>> upperHikerBoundIntersections =
            this->world.getTerrain().getAllIntersections(upperIntersectionLine);

        if (!intersections.empty() && intersections.front()->intersection != pos) {
            pos = intersections.front()->intersection;
            hiker.setYVelocity(0.f);
            hiker.setHikerMoving();
            hiker.setLastJump(0.0);
            // } else if (!upperHikerBoundIntersections.empty() && intersections.front()->intersection != pos +
            // heightDelta) {
            //    pos = {upperHikerBoundIntersections.front()->intersection.x,
            //           upperHikerBoundIntersections.front()->intersection.y - hikerConstants.hikerHeight};
            //    hiker.setYVelocity(-0.01f);
        } else {
            pos = newPos;
        }
    } else {
        Vector newPos = this->calculateTheoreticalNextHikerPosition(knockbackVector);
        Vector direction = newPos - pos;

        if (direction.x == 0 || direction.computeSlope() > this->hikerConstants.maxClimbableSlope) {
            if (direction.y < 0) {
                spdlog::debug("falling off terrain");
                // Fall off terrain
                pos.x = newPos.x;
                hiker.setHikerInAir();
                hiker.setLastJump(0.0);
            } else {
                spdlog::debug("standing still");
                // Stand still
                // TODO: Slide downhill
            }
        } else {
            spdlog::debug("moving along terrain");
            // Move along terrain
            // const floatType length = direction.length();
            // const floatType speedFactor = getSpeedFactor(direction.computeSlope());
            //// TODO this speed formula is sus, lets' just give him a constant speed (length of vel vector)
            // pos.x += (this->deltaT * std::abs(vel.x * speedFactor) / length) * direction.x + knockback;
            // pos.y = this->world.getTerrain().mapHeightToTerrain(pos);
            pos = newPos;
        }
    }
    if (pos.x > this->world.getMonster().getXPosition() + barriersConstants.playerRightBarrierOffset) {
        pos.x = this->world.getMonster().getXPosition() + barriersConstants.playerRightBarrierOffset;
        pos.y = this->world.getTerrain().mapHeightToTerrain(pos);
    }
    hiker.setPosition(pos);
}

floatType Positioner::getSpeedFactor(floatType slope) const {
    if (slope <= hikerConstants.slowestNegSlope) {
        return hikerConstants.minSpeedNegSlope;
    }
    if (slope <= hikerConstants.fastestNegSlope) {
        return Vector::linearInterpolation(slope, {hikerConstants.slowestNegSlope, hikerConstants.minSpeedNegSlope},
                                           {hikerConstants.fastestNegSlope, hikerConstants.maxSpeedNegSlope});
    }
    if (slope <= 0) {
        return Vector::linearInterpolation(slope, {hikerConstants.fastestNegSlope, hikerConstants.maxSpeedNegSlope},
                                           {0, 1});
    }
    if (slope <= hikerConstants.slowestPosScope) {
        return Vector::linearInterpolation(slope, {0, 1},
                                           {hikerConstants.slowestPosScope, hikerConstants.minSpeedPosSlope});
    }

    return hikerConstants.minSpeedPosSlope;
}

Vector Positioner::calculateTheoreticalNextHikerPosition(const Vector knockback) const {
    Hiker &hiker = this->world.getHiker();
    const auto vel = hiker.getVelocity();
    const auto oldPos = hiker.getPosition();
    floatType nextXPos;
    floatType nextYPos;
    spdlog::debug("Hiker direction: {}", hiker.getHikerMovement().getDirection());
    if (hiker.getHikerMovement().getDirection() != HikerMovement::Direction::NEUTRAL) {
        nextXPos = vel.x * this->deltaT + oldPos.x;
        nextYPos = this->world.getTerrain().mapHeightToTerrain({nextXPos, oldPos.y});
        Vector newPos = {nextXPos, nextYPos};

        auto direction = newPos - oldPos;
        const floatType length = direction.length();
        const floatType speedFactor = getSpeedFactor(direction.computeSlope());
        nextXPos = oldPos.x + (this->deltaT * std::abs(vel.x * speedFactor) / length) * direction.x + knockback.x;
        nextYPos = this->world.getTerrain().mapHeightToTerrain({nextXPos, oldPos.y + knockback.y});
    } else {
        if (knockback.x != 0) {
            nextXPos = oldPos.x + knockback.x;
            nextYPos = this->world.getTerrain().mapHeightToTerrain({nextXPos, oldPos.y + knockback.y});
        } else {
            nextXPos = oldPos.x;
            nextYPos = oldPos.y;
        }
    }
    Vector newPos = {nextXPos, nextYPos};
    return newPos;
};