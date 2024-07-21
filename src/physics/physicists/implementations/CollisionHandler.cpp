//
// Created by bjoern on 5/21/24.
//

#include "../CollisionHandler.hpp"
#include "../../../output/graphics/Renderer.h"

#include <iostream>
#include <mutex>

CollisionHandler::CollisionHandler(World &world, CollisionDetector &collisionDetector, AudioService &audioService,
                                   Renderer &renderer, GameConstants gameConstants)
    : world(world), collisionDetector(collisionDetector), audioService(audioService), renderer(renderer),
      gameConstants(gameConstants), hapticsService(HapticsService::getInstance()), deltaT(1) {}

void CollisionHandler::handleCollisions() {
    this->playerCollisions();
    // TODO rock rock collision and rock terrain collision should maybe not be separated
    // TODO => One big system of PDEs with boundary conditions
    this->rockTerrainCollisions();
    this->rockRockCollisions();
}

void CollisionHandler::setDeltaT(const floatType deltaT) { this->deltaT = deltaT; }

Vertex CollisionHandler::getClosestVertex(Rock &rock) const {
    const auto pos = rock.getPosition();
    const floatType rad = rock.getRadius();
    const floatType xMin = pos.x - rad;
    const floatType xMax = pos.x + rad;
    const Mountain &mountain = this->world.getMountain();

    return Vertex({0, 0});
}

void CollisionHandler::playerCollisions() const {
    // TODO parallelize and make efficient with linked cell or sth
    for (auto &rock : this->world.getRocks()) {
        if (this->collisionDetector.isPlayerHitByRock(rock)) {
            // TODO player hit sound and rock explosion (texture, later actual explosion) should be somewhere else
            this->audioService.playSound("rock-smash");
            this->audioService.playSound("boom");
            const int rockDmg = this->rockDamage(rock);
            spdlog::debug("Player has hit with rock damage: {}", rockDmg);
            HapticsService::rockRumble(rockDmg);
            this->world.getHiker().setHealthPoints(this->world.getHiker().getHealthPoints() - rockDmg);
            this->world.getHiker().setIsHit(true);
            // TODO knockback issue
            this->world.getHiker().setHitInformation({rock.getRadius(), rock.getVelocity().x, 0});
            rock.setShouldBeDestroyed(true);
        }
    }
}

int CollisionHandler::rockDamage(Rock &rock) const {
    const auto relativeRockSize = std::abs(49 * (rock.getRadius() - gameConstants.rockConstants.minRockSize));
    const auto rockSizeRange = (gameConstants.rockConstants.maxRockSize - gameConstants.rockConstants.minRockSize) + 1;
    const auto rockSizeFactor = static_cast<int>((relativeRockSize / rockSizeRange));
    const auto velocityFactor =
        static_cast<int>(1 + rock.getVelocity().length() / gameConstants.rockConstants.velocityCap);
    return rockSizeFactor * velocityFactor;
}

void CollisionHandler::rockTerrainCollisions() {
    for (auto &rock : this->world.getRocks()) {
        // TODO don't create too many copies, but will be changed later anyways
        Rock virtualRock = this->getNextState(rock);
        // const auto closestVertex = this->getClosestVertex(virtualRock);
        //  TODO Needs to be changed, I just want to test the mountain rendering
        if (this->world.getMountain().isInRange(virtualRock.getPosition().x)) {
            if (virtualRock.getPosition().y - this->world.getMountain().calculateYPos(virtualRock.getPosition().x) <
                virtualRock.getRadius()) {
                this->rockTerrainCollision(rock);
            }
        }
    }
}

void CollisionHandler::rockTerrainCollision(Rock &rock) const {
    auto vel = rock.getVelocity(); // TODO play terrain collision sound
    auto pos = rock.getPosition();
    const auto rad = rock.getRadius();
    floatType angularVelocity = rock.getAngularVelocity();
    floatType angularOffset = rock.getAngularOffset();
    // const auto normal = this->collisionDetector.getNormal(closestVertex.index, pos);
    const auto normal = this->collisionDetector.getNormal(pos);
    vel = vel.reflectOnNormal(normal, gameConstants.physicsConstants.rockTerrainDamping);
    const auto mass = std::pow(rad, 2);
    const Vector parallelVector = {-normal.y, normal.x};
    const auto velocityParallel = vel * parallelVector;
    angularVelocity +=
        static_cast<floatType>((gameConstants.rockConstants.gamma) * (velocityParallel - angularVelocity * rad) / mass);
    const auto maxAngularVelocity = gameConstants.rockConstants.maxAngularVelocity;
    if (angularVelocity >= maxAngularVelocity) {
        angularVelocity = maxAngularVelocity;
    }
    if (angularVelocity <= -maxAngularVelocity) {
        angularVelocity = -maxAngularVelocity;
    }
    angularOffset += this->deltaT * angularVelocity;
    rock.setVelocity(vel); //  TODO maybe later explode rock here
    rock.setAngularVelocity(angularVelocity);
    rock.setAngularOffset(angularOffset);
    auto shake =
        (rock.getVelocity().length() * rock.getRadius() * this->gameConstants.visualConstants.rumbleIntensity) /
        gameConstants.rockConstants.velocityCap;
    this->renderer.setShake(shake);
}

void CollisionHandler::rockRockCollisions() {
    for (auto &rock1 : this->world.getRocks()) {
        for (auto &rock2 : this->world.getRocks()) {
            // TODO remove unnecessary copies as far as possible
            Rock vRock1 = this->getNextState(rock1);
            Rock vRock2 = this->getNextState(rock2);
            if (CollisionDetector::rocksCollide(vRock1, vRock2)) {
                // TODO play rock collision sounds
                rockRockCollision(rock1, rock2);

                // TODO maybe explode rocks
            }
        }
    }
}
// NOLINTBEGIN
void CollisionHandler::rockRockCollision(Rock &rock1, Rock &rock2) {
    const floatType mass1 = powf(rock1.getRadius(), 2);
    const floatType mass2 = powf(rock1.getRadius(), 2);
    Vector vel1 = rock1.getVelocity();
    Vector vel2 = rock2.getVelocity();
    const Vector pos1 = rock1.getPosition();
    const Vector pos2 = rock2.getPosition();
    const Vector velDiff = vel1 - vel2;
    const Vector posDiff = pos1 - pos2;
    floatType angularVelocity1 = rock1.getAngularVelocity();
    floatType angularOffset1 = rock1.getAngularOffset();
    floatType angularVelocity2 = rock2.getAngularVelocity();
    floatType angularOffset2 = rock2.getAngularOffset();
    const floatType distanceSq = posDiff * posDiff;
    const floatType totalMass = mass1 + mass2;
    vel1 -=
        posDiff * 2 * mass2 * (velDiff * posDiff) / (distanceSq * totalMass + gameConstants.physicsConstants.epsilon);
    vel2 +=
        posDiff * 2 * mass1 * (velDiff * posDiff) / (distanceSq * totalMass + gameConstants.physicsConstants.epsilon);
    const Vector normal = {pos2.y - pos1.y, pos1.x - pos2.x};
    const floatType relativeSurfaceVelocity =
        angularVelocity2 * rock2.getRadius() - angularVelocity1 * rock1.getRadius();
    angularVelocity1 +=
        gameConstants.rockConstants.gamma * std::abs((normal * vel1)) * relativeSurfaceVelocity / (2 * mass1);
    angularVelocity2 -=
        gameConstants.rockConstants.gamma * std::abs((normal * vel2)) * relativeSurfaceVelocity / (2 * mass2);
    angularVelocity1 = capAngularVelocity(angularVelocity1);
    angularVelocity2 = capAngularVelocity(angularVelocity2);
    rock1.setVelocity(vel1);
    rock2.setVelocity(vel2);
    rock1.setAngularVelocity(angularVelocity1);
    rock2.setAngularVelocity(angularVelocity2);
    rock1.setAngularOffset(angularOffset1);
    rock2.setAngularOffset(angularOffset2);
};
// NOLINTEND
Rock CollisionHandler::getNextState(Rock &rock) const {
    const auto pos = rock.getPosition();
    const auto newPos = pos + rock.getVelocity() * this->deltaT;
    const floatType newAngularOffset = rock.getAngularOffset() + rock.getAngularVelocity() * this->deltaT;
    return {newPos, rock.getVelocity(), rock.getAngularVelocity(), newAngularOffset, rock.getRadius()};
}

floatType CollisionHandler::capAngularVelocity(const floatType angVel) const {
    const auto maxAngularVelocity = gameConstants.rockConstants.maxAngularVelocity;
    if (angVel >= maxAngularVelocity) {
        return maxAngularVelocity;
    }
    if (angVel <= -maxAngularVelocity) {
        return -maxAngularVelocity;
    }
    return angVel;
}
