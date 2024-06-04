//
// Created by bjoern on 5/21/24.
//

#include "CollisionHandler.hpp"

#include <iostream>
#include <mutex>

CollisionHandler::CollisionHandler()
    : world(World::getInstance()), collisionDetector(CollisionDetector::getInstance()), deltaT(1) {
    std::cout << "CollisionHandler gets constructed" << std::endl;
}

CollisionHandler::~CollisionHandler() { std::cout << "CollisionHandler gets deconstructed" << std::endl; }

void CollisionHandler::handleCollisions() {
    this->playerCollisions();
    // TODO rock rock collision and rock terrain collision should maybe not be separated
    // TODO => One big system of PDEs with boundary conditions
    // TODO also, why is there a lookahead for rock-rock, but not for rock-terrain?
    this->rockTerrainCollisions();
    this->rockRockCollisions();
}

void CollisionHandler::setDeltaT(const float deltaT) { this->deltaT = deltaT; }

Vertex CollisionHandler::getClosestVertex(RockClass &rock) const {
    // TODO we need to define floatType in one of our classes
    const auto pos = rock.getPosition();
    const float rad = rock.getRadius();
    const floatType xMin = pos.x - rad;
    const floatType xMax = pos.x + rad;
    const MountainClass &mountain = this->world.getMountain();
    auto interval = MountainClass::getRelevantMountainSection(xMin, xMax);

    auto closestIndex = interval.startIndex;
    floatType closestDistance = mountain.getVertex(interval.startIndex).distanceTo(pos);

    for (auto j = interval.startIndex; j < interval.endIndex; j++) {
        auto mountainVertex = mountain.getVertex(j);
        auto currentDist = mountainVertex.distanceTo(pos);

        if (currentDist < closestDistance) {
            closestDistance = currentDist;
            closestIndex = j;
        }
    }
    return Vertex({closestIndex, closestDistance});
}

void CollisionHandler::playerCollisions() const {
    // TODO parallelize and make efficient with linked cell or sth
    for (auto &rock : this->world.getRocks()) {
        if (this->collisionDetector.isPlayerHitByRock(rock)) {
            // TODO player hit sound and rock explosion (texture, later actual explosion) should be somewhere else
            // TODO check damage formula and make dependent on rock type
            const int rockDmg = rockDamage(rock);
            // TODO remove prints ig
            std::cout << "Player hit! Dmg: " << rockDmg << std::endl;
            // TODO remove rumble from here or call it here or tell something to the output??
            this->world.getHiker().setHealthPoints(this->world.getHiker().getHealthPoints() - rockDmg);
            this->world.getHiker().setIsHit(true);
            // TODO this is bad, it calculates only a single hit for the knockback, multiple hits are ignored
            // TODO why is knockback only in x direction
            this->world.getHiker().setHitInformation({rock.getRadius(), rock.getVelocity().x, 0});
            // TODO either destruct method in rock or destruct in world
            rock.setShouldBeDestroyed(true);
            // TODO rumble and end game should be checked somewhere else I think
        }
    }
}

int CollisionHandler::rockDamage(RockClass &rock) {
    return static_cast<int>((std::abs(49 * (rock.getRadius() - MIN_ROCK_SIZE)) / (MAX_ROCK_SIZE - MIN_ROCK_SIZE) + 1) *
                            (1 + rock.getVelocity().length() / VELOCITY_CAP));
}

void CollisionHandler::rockTerrainCollisions() { // const {
    for (auto &rock : this->world.getRocks()) {
        // TODO are we sure about a list of rock pointers?
        // TODO don't create too many copies, but will be changed later anyways
        RockClass virtualRock = this->getNextState(rock);
        const auto closestVertex = this->getClosestVertex(virtualRock);
        if (closestVertex.distance <= rock.getRadius()) {
            this->rockTerrainCollision(rock, closestVertex);
            // std::cout << "Collision " << closestVertex.distance << " | " << getClosestVertex(rock).distance
            //<< std::endl;
        }
    }
}

// NOLINTBEGIN
// TODO we shouldn't update positions here, just velocities
void CollisionHandler::rockTerrainCollision(RockClass &rock, const Vertex closestVertex) const {
    // TODO play terrain collision sound somewhere else
    // const auto vertexPosition = this->world.getMountain().getVertex(closestVertex.index);
    auto vel = rock.getVelocity();
    auto pos = rock.getPosition();
    const auto rad = rock.getRadius();
    auto rot = rock.getRotation();
    const auto normal = this->collisionDetector.getNormal(closestVertex.index, pos);
    vel = vel.reflectOnNormal(normal);
    // TODO will be changed anyways
    /*const floatType vertexNormal = vertexPosition * normal;
    const floatType positionNormal = pos * normal;
    const floatType velocityNormal = vel * normal;*/
    // TODO I think we don't need this here
    /*const floatType terrainExitTime = (rad + vertexNormal - positionNormal) / velocityNormal;
    pos += vel * terrainExitTime + EPSILON;*/
    const auto mass = std::pow(rad, 2);
    const Vector parallelVector = {-normal.y, normal.x};
    const auto velocityParallel = vel * parallelVector;
    rot.angular_velocity += static_cast<float>((GAMMA) * (velocityParallel - rot.angular_velocity * rad) / mass);
    if (rot.angular_velocity >= MAX_ANGULAR_VELOCITY) {
        rot.angular_velocity = MAX_ANGULAR_VELOCITY;
    }
    if (rot.angular_velocity <= -MAX_ANGULAR_VELOCITY) {
        rot.angular_velocity = -MAX_ANGULAR_VELOCITY;
    }
    rot.angular_offset += this->deltaT * rot.angular_velocity;
    // rock.setPosition(pos);
    //   TODO slow down jimbo, maybe later explode rock here
    rock.setVelocity(vel);
    rock.setRotation(rot);
}
// NOLINTEND

void CollisionHandler::rockRockCollisions() {
    for (auto &rock1 : this->world.getRocks()) {
        for (auto &rock2 : this->world.getRocks()) {
            // TODO remove unnecessary copies as far as possible
            RockClass vRock1 = this->getNextState(rock1);
            RockClass vRock2 = this->getNextState(rock2);
            if (CollisionDetector::rocksCollide(vRock1, vRock2)) {
                // TODO As always, this should be done somewhere else
                /*if (!IsSoundPlaying(rock_collision_sound) && app_info->playerAlive) {
                    PlaySound(rock_collision_sound);
                }*/
                rockRockCollision(rock1, rock2);

                // TODO slow down jimbo, we can explode stuff later
                /*if (it.entity(i).has<Exploding>()) {
                        std::cout << "eskalation" << std::endl;
                        explodeRock(it.world(), it.entity(i), 1);
                    }*/
            }
        }
    }
}

void CollisionHandler::rockRockCollision(RockClass &rock1, RockClass &rock2) {
    const floatType mass1 = powf(rock1.getRadius(), 2);
    const floatType mass2 = powf(rock1.getRadius(), 2);
    Vector vel1 = rock1.getVelocity();
    Vector vel2 = rock2.getVelocity();
    const Vector pos1 = rock1.getPosition();
    const Vector pos2 = rock2.getPosition();
    const Vector velDiff = vel1 - vel2;
    const Vector posDiff = pos1 - pos2;
    auto [angularVelocity1, angularOffset1] = rock1.getRotation();
    auto [angularVelocity2, angularOffset2] = rock2.getRotation();
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
    rock1.setVelocity(vel2);
    const Rotation rot1 = {angularVelocity1, angularOffset1};
    const Rotation rot2 = {angularVelocity2, angularOffset2};
    rock1.setRotation(rot1);
    rock1.setRotation(rot2);
};

RockClass CollisionHandler::getNextState(RockClass &rock) const {
    const auto pos = rock.getPosition();
    const auto newPos = pos + rock.getVelocity() * this->deltaT;
    const auto rot = rock.getRotation();
    auto newRot = rot;
    // TODO Why do we still have a Rotation struct with shitty denominators and why are these values not rock attributes
    newRot.angular_offset += rot.angular_velocity * this->deltaT;
    return {newPos, rock.getVelocity(), newRot, rock.getRadius()};
}

float CollisionHandler::capAngularVelocity(const float angVel) {
    if (angVel >= MAX_ANGULAR_VELOCITY) {
        return MAX_ANGULAR_VELOCITY;
    }
    if (angVel <= -MAX_ANGULAR_VELOCITY) {
        return -MAX_ANGULAR_VELOCITY;
    }
    return angVel;
}
