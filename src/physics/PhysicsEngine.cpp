
//
// Created by bjoern on 5/21/24.
//

#include "PhysicsEngine.hpp"

#include <iostream>
#include <mutex>
PhysicsEngine::PhysicsEngine(World &world, Spawner &spawner, PhysicsConstants &physicsConstants,
                             GameEventProcessor &eventProcessor, Accelerator &accelerator, Positioner &positioner,
                             CollisionDetector &collisionDetector, CollisionHandler &collisionHandler,
                             Interpolator &interpolator, Destructor &destructor)
    : world(world), accumulator(0.f), timeLastUpdate(0.f), spawner(spawner), physicsConstants(physicsConstants),
      deltaT(physicsConstants.physicsDeltaT), eventProcessor(eventProcessor), accelerator(accelerator),
      positioner(positioner), collisionDetector(collisionDetector), collisionHandler(collisionHandler),
      interpolator(interpolator), destructor(destructor) {
    this->setDeltaT(this->physicsConstants.physicsDeltaT);
}

void PhysicsEngine::update(std::queue<GameEvent> &events) {
    auto currentTime = static_cast<floatType>(GetTime()); // TODO should this be GetFrameTime()?
    const floatType frameTime = currentTime - this->timeLastUpdate;
    this->timeLastUpdate = currentTime;
    // TODO fine adjustment of max frame time to avoid spiral of death in case of lag
    this->accumulator += frameTime > 100 * this->deltaT ? 100 * this->deltaT : frameTime;
    this->eventProcessor.addEvents(events);
    while (this->accumulator >= this->deltaT) {
        this->updateTimeStep();
        this->accumulator -= this->deltaT;
        // std::cout << "A Physics step occurred" << std::endl;
    }
    const floatType alpha = this->accumulator / this->deltaT;
    this->interpolator.interpolate(alpha);
    this->eventProcessor.clearRepeatedEvents();
}

floatType PhysicsEngine::getDeltaT() const { return this->deltaT; }

void PhysicsEngine::setDeltaT(const floatType deltaT) {
    this->deltaT = deltaT;
    this->accelerator.setDeltaT(deltaT);
    this->positioner.setDeltaT(deltaT);
    this->collisionHandler.setDeltaT(deltaT);
}

void PhysicsEngine::updateTimeStep() {
    this->spawner.spawn();
    this->eventProcessor.processEvents();
    this->accelerator.accelerate();
    // TODO check placement of collision detection
    this->collisionDetector.detectCollisions();
    this->collisionHandler.handleCollisions();
    this->positioner.updatePositions();
    this->destructor.destruct(); // TODO mountain chunks should probably also be destructed here
}