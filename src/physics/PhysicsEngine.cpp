//
// Created by bjoern on 5/21/24.
//

#include "PhysicsEngine.hpp"

PhysicsEngine *PhysicsEngine::physicsEngine = nullptr;

PhysicsEngine *PhysicsEngine::getPhysicsEngine(const float deltaT, World &world) {
    if (physicsEngine == nullptr) {
        physicsEngine = new PhysicsEngine(deltaT, world);
    }
    return physicsEngine;
}

PhysicsEngine::PhysicsEngine(const float deltaT, World &world) : deltaT(deltaT), world(world) {
    this->inputHandler = new InputHandler();
    this->eventProcessor = new EventProcessor(world, *this->inputHandler);
    this->accelerator = new Accelerator(world, deltaT);
    this->positioner = new Positioner(world, deltaT);
    this->collisionDetector = new CollisionDetector(world);
    this->collisionHandler = new CollisionHandler(world, *this->collisionDetector, this->deltaT);
    this->interpolator = new Interpolator(world);
    this->destructor = new Destructor(world);
    this->spawner = new Spawner(world);
}

void PhysicsEngine::update(const float frameTime) {
    // TODO fine adjustment of max frame time to avoid spiral of death in case of lag
    this->accumulator += frameTime > 100 * this->deltaT ? 100 * this->deltaT : frameTime;
    while (this->accumulator >= this->deltaT) {
        this->updateTimeStep();
        this->accumulator -= this->deltaT;
    }
    const float alpha = this->accumulator / this->deltaT;
    this->interpolator->interpolate(alpha);
}

void PhysicsEngine::updateTimeStep() const {
    // TODO spawn stuff
    // TODO should velocities be updated by processEvents or only in acceleate
    this->spawner->spawn();
    this->eventProcessor->processEvents();
    this->accelerator->accelerate();
    // TODO check placement of collision detection
    this->collisionDetector->detectCollisions();
    this->collisionHandler->handleCollisions();
    this->positioner->updatePositions();
    this->destructor->destruct(); // TODO mountain chunks should probably also be destructed here
}
