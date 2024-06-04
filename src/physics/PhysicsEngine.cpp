
//
// Created by bjoern on 5/21/24.
//

#include "PhysicsEngine.hpp"

#include <iostream>
#include <mutex>

PhysicsEngine::PhysicsEngine()
    : deltaT(PHYSICS_DELTA_T), world(World::getInstance()), accumulator(0.f), timeLastUpdate(0.f),
      spawner(Spawner::getInstance()), eventProcessor(EventProcessor::getInstance()),
      accelerator(Accelerator::getInstance()), positioner(Positioner::getInstance()),
      collisionDetector(CollisionDetector::getInstance()), collisionHandler(CollisionHandler::getInstance()),
      interpolator(Interpolator::getInstance()), destructor(Destructor::getInstance()) {
    this->setDeltaT(this->deltaT);
    std::cout << "PhysicsEngine gets constructed" << std::endl;
}

void PhysicsEngine::update(std::queue<GameEvent> &events) {
    auto currentTime = static_cast<float>(GetTime()); // TODO should this be GetFrameTime()?
    const float frameTime = currentTime - this->timeLastUpdate;
    this->timeLastUpdate = currentTime;
    // TODO fine adjustment of max frame time to avoid spiral of death in case of lag
    this->accumulator += frameTime > 100 * this->deltaT ? 100 * this->deltaT : frameTime;
    this->eventProcessor.setEventQueue(events);
    while (this->accumulator >= this->deltaT) {
        this->updateTimeStep();
        this->accumulator -= this->deltaT;
        // std::cout << "A Physics step occurred" << std::endl;
    }
    const float alpha = this->accumulator / this->deltaT;
    this->interpolator.interpolate(alpha);
}

float PhysicsEngine::getDeltaT() const { return this->deltaT; }

void PhysicsEngine::setDeltaT(const float deltaT) {
    this->deltaT = deltaT;
    this->accelerator.setDeltaT(deltaT);
    this->positioner.setDeltaT(deltaT);
    this->collisionHandler.setDeltaT(deltaT);
}

void PhysicsEngine::updateTimeStep() const {
    this->spawner.spawn();
    this->eventProcessor.processEvents();
    this->accelerator.accelerate();
    // TODO check placement of collision detection
    this->collisionDetector.detectCollisions();
    this->collisionHandler.handleCollisions();
    this->positioner.updatePositions();
    this->destructor.destruct(); // TODO mountain chunks should probably also be destructed here
}

PhysicsEngine::~PhysicsEngine() {
    // TODO delete the other singletons (all singletons need a static destructor)
    std::cout << "PhysicsEngine gets deconstructed" << std::endl;
}
