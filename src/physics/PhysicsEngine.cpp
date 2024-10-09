
//
// Created by bjoern on 5/21/24.
//

#include "PhysicsEngine.hpp"

#include <iostream>
#include <mutex>
PhysicsEngine::PhysicsEngine(World &world,
                             Spawner &spawner,
                             PhysicsConstants &physicsConstants,
                             GameEventProcessor &eventProcessor,
                             Accelerator &accelerator,
                             Positioner &positioner,
                             CollisionDetector &collisionDetector,
                             CollisionHandler &collisionHandler,
                             Interpolator &interpolator,
                             Destructor &destructor)
    : deltaT(physicsConstants.physicsDeltaT),
      world(world),
      physicsConstants(physicsConstants),
      timeLastUpdate(0.f),
      accumulator(0.f),
      spawner(spawner),
      eventProcessor(eventProcessor),
      accelerator(accelerator),
      positioner(positioner),
      collisionDetector(collisionDetector),
      collisionHandler(collisionHandler),
      interpolator(interpolator),
      destructor(destructor) {
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
        spdlog::debug("Physics Step");
    }
    spdlog::debug("Frame");
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
    this->positioner.updatePositions();
    this->collisionHandler.handleCollisions();
    this->destructor.destruct();
}

void PhysicsEngine::reset() { this->spawner.reset(); }
