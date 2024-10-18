//
// Created by bjoern on 10/9/24.
//

#include "Simulation.h"
#include "../entities/World.h"
#include "../spawner/PolygonGenerator.h"

#include <cmath>

Simulation::Simulation(World &world, const SimulationProperties &simProperties)
    : simProperties(simProperties), world(world) {
    // Creates a dodekagon
    std::vector<Vector> rockVertices{12};
    rockVertices.at(0) = {0, -1.f};
    for (size_t i = 1; i < 12; i++) {
        const Vector newVertex = rockVertices[i - 1].rotateByAngle(M_PI / 6);
        rockVertices.at(i) = (newVertex);
    }
    this->rockSpawnState = DynamicProperties{this->simProperties.dropPosition, 0.f, 0.f, {0.f, 0.f}, 0.f};
    this->rockBlueprint = std::make_shared<Rock>(
        Rock(this->simProperties.dropPosition,
             PolygonGenerator::generatePolygonFromVertices(rockVertices, this->simProperties.dropPosition, 1.0),
             this->rockSpawnState, NORMAL_ROCK));
}

void Simulation::init() {
    this->startTime = GetTime();
    this->timeOfLastSpawn = this->startTime;
    this->rockSpawnState.linearMomentum = this->getLinearMomentumFromSpeed();
    this->numberOfSpawnedRocks = 0;
}

bool Simulation::tick() {
    const double time = GetTime();
    bool simStillRunning = true;
    if (this->shouldSpawnAgain(time)) {
        this->spawnRock();
        this->timeOfLastSpawn = time;
    }
    if (time - this->startTime > this->simProperties.duration) {
        simStillRunning = false;
        spdlog::info("Simulation stopped");
    }
    return simStillRunning;
}

Vector Simulation::getLinearMomentumFromSpeed() const {
    return Vector{(4.f / static_cast<floatType>(this->simProperties.speed) * this->rockBlueprint->getMass()), 0};
}

void Simulation::spawnRock() {
    const auto newRock = std::make_shared<Rock>(
        this->simProperties.dropPosition, this->rockBlueprint->getBodySpaceVertices(),
        this->rockBlueprint->getTextureCoordinates(), this->rockBlueprint->getMass(), this->rockBlueprint->getDensity(),
        this->rockBlueprint->getMomentOfInertia(), this->rockSpawnState);
    this->world.addRock(newRock);
    this->numberOfSpawnedRocks++;
}

bool Simulation::shouldSpawnAgain(const double time) const {
    return time - this->timeOfLastSpawn > this->simProperties.speed &&
           this->numberOfSpawnedRocks < this->simProperties.maxNumberOfRocks;
}