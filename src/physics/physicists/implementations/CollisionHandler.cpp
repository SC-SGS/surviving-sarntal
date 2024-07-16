//
// Created by bjoern on 5/21/24.
//

#include "../CollisionHandler.hpp"
#include "../../../output/graphics/Renderer.h"

#include <iostream>
#include <mutex>

CollisionHandler::CollisionHandler(World &world, CollisionDetector &collisionDetector, AudioService &audioService,
                                   Renderer &renderer)
    : world(world), collisionDetector(collisionDetector), audioService(audioService), renderer(renderer),
      hapticsService(HapticsService::getInstance()), deltaT(1) {}

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
    // auto interval = Mountain::getRelevantMountainSection(xMin, xMax);
    //
    // auto closestIndex = interval.startIndex;
    // floatType closestDistance = mountain.getVertex(interval.startIndex).distanceTo(pos);
    //
    // for (auto j = interval.startIndex; j < interval.endIndex; j++) {
    //     auto mountainVertex = mountain.getVertex(j);
    //     auto currentDist = mountainVertex.distanceTo(pos);
    //
    //     if (currentDist < closestDistance) {
    //         closestDistance = currentDist;
    //         closestIndex = j;
    //     }
    // }
    return Vertex({0, 0});
    // return Vertex({closestIndex, closestDistance});
}

void CollisionHandler::playerCollisions() const {
    // TODO parallelize and make efficient with linked cell or sth
    for (auto &rock : this->world.getRocks()) {
        if (this->collisionDetector.isPlayerHitByRock(rock)) {
            // TODO player hit sound and rock explosion (texture, later actual explosion) should be somewhere else
            this->audioService.playSound("rock-smash");
            this->audioService.playSound("boom");
            const int rockDmg = rockDamage(rock);
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

int CollisionHandler::rockDamage(Rock &rock) {
    return static_cast<int>((std::abs(49 * (rock.getRadius() - MIN_ROCK_SIZE)) / (MAX_ROCK_SIZE - MIN_ROCK_SIZE) + 1) *
                            (1 + rock.getVelocity().length() / VELOCITY_CAP));
}

void CollisionHandler::rockTerrainCollisions() { // const {
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
        // if (closestVertex.distance <= rock.getRadius()) {
        //     this->rockTerrainCollision(rock, closestVertex);
        //     // std::cout << "Collision " << closestVertex.distance << " | " << getClosestVertex(rock).distance
        //     //<< std::endl;
        // }
    }
}

void CollisionHandler::rockTerrainCollision(Rock &rock) const {
    // TODO play terrain collision sound
    auto vel = rock.getVelocity();
    auto pos = rock.getPosition();
    const auto rad = rock.getRadius();
    floatType angularVelocity = rock.getAngularVelocity();
    floatType angularOffset = rock.getAngularOffset();
    // const auto normal = this->collisionDetector.getNormal(closestVertex.index, pos);
    const auto normal = this->collisionDetector.getNormal(pos);
    vel = vel.reflectOnNormal(normal);
    const auto mass = std::pow(rad, 2);
    const Vector parallelVector = {-normal.y, normal.x};
    const auto velocityParallel = vel * parallelVector;
    angularVelocity += static_cast<floatType>((GAMMA) * (velocityParallel - angularVelocity * rad) / mass);
    if (angularVelocity >= MAX_ANGULAR_VELOCITY) {
        angularVelocity = MAX_ANGULAR_VELOCITY;
    }
    if (angularVelocity <= -MAX_ANGULAR_VELOCITY) {
        angularVelocity = -MAX_ANGULAR_VELOCITY;
    }
    angularOffset += this->deltaT * angularVelocity;
    //  TODO maybe later explode rock here
    rock.setVelocity(vel);
    rock.setAngularVelocity(angularVelocity);
    rock.setAngularOffset(angularOffset);

    this->renderer.setShake((rock.getVelocity().length() * rock.getRadius() * VISUAL_RUMBLE_INTENSITY) / VELOCITY_CAP);
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
    vel1 -= posDiff * 2 * mass2 * (velDiff * posDiff) / (distanceSq * totalMass + EPSILON);
    vel2 += posDiff * 2 * mass1 * (velDiff * posDiff) / (distanceSq * totalMass + EPSILON);
    const Vector normal = {pos2.y - pos1.y, pos1.x - pos2.x};
    const floatType relativeSurfaceVelocity =
        angularVelocity2 * rock2.getRadius() - angularVelocity1 * rock1.getRadius();
    angularVelocity1 += GAMMA * std::abs((normal * vel1)) * relativeSurfaceVelocity / (2 * mass1);
    angularVelocity2 -= GAMMA * std::abs((normal * vel2)) * relativeSurfaceVelocity / (2 * mass2);
    angularVelocity1 = capAngularVelocity(angularVelocity1);
    angularVelocity2 = capAngularVelocity(angularVelocity2);
    rock1.setVelocity(vel1);
    rock2.setVelocity(vel2);
    rock1.setAngularVelocity(angularVelocity1);
    rock2.setAngularVelocity(angularVelocity2);
    rock1.setAngularOffset(angularOffset1);
    rock2.setAngularOffset(angularOffset2);
};

Rock CollisionHandler::getNextState(Rock &rock) const {
    const auto pos = rock.getPosition();
    const auto newPos = pos + rock.getVelocity() * this->deltaT;
    const floatType newAngularOffset = rock.getAngularOffset() + rock.getAngularVelocity() * this->deltaT;
    return {newPos, rock.getVelocity(), rock.getAngularVelocity(), newAngularOffset, rock.getRadius()};
}

floatType CollisionHandler::capAngularVelocity(const floatType angVel) {
    if (angVel >= MAX_ANGULAR_VELOCITY) {
        return MAX_ANGULAR_VELOCITY;
    }
    if (angVel <= -MAX_ANGULAR_VELOCITY) {
        return -MAX_ANGULAR_VELOCITY;
    }
    return angVel;
}
