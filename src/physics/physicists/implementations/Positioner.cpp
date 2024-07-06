//
// Created by bjoern on 5/21/24.
//

#include "../Positioner.hpp"

#include <iostream>
#include <mutex>

Positioner::Positioner(World &world) : world(world), deltaT(1){};

void Positioner::updatePositions() const {
    this->updateMonsterPosition();
    this->updateWorldBorderPosition();
    this->updateRockPositions();
    this->updateHikerPosition();
}

void Positioner::setDeltaT(const floatType deltaT) { this->deltaT = deltaT; };

void Positioner::updateMonsterPosition() const {
    const auto xPos = this->world.getMonster().getXPosition();
    const auto newXPos = xPos + KILL_BAR_VELOCITY * this->deltaT;
    const auto newYPos = this->world.getMountain().getYPosFromX(xPos);
    Vector newPos = {newXPos, newYPos};
    this->world.getMonster().setPosition(newPos);
}

void Positioner::updateWorldBorderPosition() const {
    const auto xMin = this->world.getMinX();
    const auto xMax = this->world.getMaxX();
    const auto delta = KILL_BAR_VELOCITY * this->deltaT;
    this->world.setMinX(xMin + delta);
    this->world.setMaxX(xMax + delta);
    // std::cout << "World border x minimum position: " << xMin << std::endl;
}

void Positioner::updateRockPositions() const {
    for (auto &rock : this->world.getRocks()) {
        auto pos = rock.getPosition();
        // std::cout << pos.x << pos.y << std::endl;
        pos += rock.getVelocity() * this->deltaT;
        rock.setPosition(pos);
        const floatType newAngularOffset = rock.getAngularOffset() + rock.getAngularVelocity() * this->deltaT;
        rock.setAngularOffset(newAngularOffset);
        // std::cout << pos.x << pos.y << std::endl;
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
        knockback = rockVelocity * KNOCKBACKCONST * radius;
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
    if (hiker.getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
        pos.x += knockback + AIR_MOVEMENT_SPEED_FACTOR * vel.x * this->deltaT;
        const auto terrainY = this->world.getMountain().getYPosFromX(pos.x);
        const auto airY = pos.y + vel.y * this->deltaT;
        if (airY > terrainY) {
            pos.y = airY;
        } else {
            pos.y = terrainY;
            hiker.setYVelocity(0.f);
            hiker.setHikerMoving();
            hiker.setLastJump(0.0);
        }
        // TODO this whole speedfactor shebang needs thorough examination and fiddling in later stages
    } else if (hiker.getHikerMovement().getDirection() != HikerMovement::Direction::NEUTRAL) {
        const auto nextXPos = vel.x * this->deltaT + pos.x;
        const auto nextYPos = this->world.getMountain().getYPosFromX(nextXPos);
        Vector direction = {nextXPos - pos.x, nextYPos - pos.y};
        const floatType length = direction.length();
        const floatType slope = direction.y / direction.x;
        const floatType speedFactor = getSpeedFactor(slope);
        // TODO this speed formula is sus, lets' just give him a constant speed (length of vel vector)
        pos.x += (this->deltaT * std::abs(vel.x * speedFactor) / length) * direction.x + knockback;
        pos.y = this->world.getMountain().getYPosFromX(pos.x);
    } else {
        // std::cout << "Knock knock mf" << std::endl;
        pos.x += knockback;
        pos.y = this->world.getMountain().getYPosFromX(pos.x);
    }
    if (pos.x > this->world.getMonster().getXPosition() + PLAYER_RIGHT_BARRIER_OFFSET) {
        pos.x = this->world.getMonster().getXPosition() + PLAYER_RIGHT_BARRIER_OFFSET;
    }
    hiker.setPosition(pos);
}

floatType Positioner::getSpeedFactor(const floatType slope) {
    if (slope <= SLOWEST_NEG_SLOPE) {
        return MIN_SPEED_NEG_SLOPE;
    }
    if (slope <= FASTEST_NEG_SCOPE) {
        return Mountain::linearInterpolation(slope, {SLOWEST_NEG_SLOPE, MIN_SPEED_NEG_SLOPE},
                                             {FASTEST_NEG_SCOPE, MAX_SPEED_NEG_SLOPE});
    }
    if (slope <= 0) {
        return Mountain::linearInterpolation(slope, {FASTEST_NEG_SCOPE, MAX_SPEED_NEG_SLOPE}, {0, 1});
    }
    if (slope <= SLOWEST_POS_SCOPE) {
        return Mountain::linearInterpolation(slope, {0, 1}, {SLOWEST_POS_SCOPE, MIN_SPEED_POS_SCOPE});
    }

    return MIN_SPEED_POS_SCOPE;
};