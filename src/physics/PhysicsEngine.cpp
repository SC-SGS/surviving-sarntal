
//
// Created by bjoern on 5/21/24.
//

#include "PhysicsEngine.hpp"

#include <iostream>
#include <mutex>

double PhysicsEngine::exponentialMovingAvg = 0;

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
    if (!gameRunning) {
        this->startEngine();
    }

    this->updateAccumulator();
    this->eventProcessor.addEvents(events);

    while (this->accumulator >= this->deltaT) {
        this->updateTimeStepAndRecord();
        this->accumulator -= this->deltaT;
    }

    const floatType alpha = this->accumulator / this->deltaT;
    this->interpolator.interpolate(alpha);
    this->eventProcessor.clearRepeatedEvents();
}

void PhysicsEngine::startEngine() {
    gameRunning = true;
    this->timeLastUpdate = static_cast<floatType>(GetTime());
}

void PhysicsEngine::updateTimeStepAndRecord() {
    const double t0Start = GetTime();
    this->updateTimeStep();
    const double t1Stop = GetTime();
    this->recordTimeStep(t0Start, t1Stop);
    spdlog::debug("Physics Step");
}

void PhysicsEngine::recordTimeStep(const double t0Start, const double t1Stop) {
    const double stepTime = t1Stop - t0Start;
    exponentialMovingAvg = exponentialMovingAvg * 0.8 + stepTime * 0.2;
    if (this->recording) {
        this->record.stepTimes.emplace_back(t0Start, stepTime);
        this->record.avgStepTime += stepTime;
    }
}

void PhysicsEngine::updateAccumulator() {
    const auto currentTime = static_cast<floatType>(GetTime());
    const floatType frameTime = currentTime - this->timeLastUpdate;
    this->timeLastUpdate = currentTime;

    const auto maxSpfTime =
        static_cast<floatType>(this->physicsConstants.maxNumberOfPhysicsStepsPerFrame) * this->deltaT;

    this->accumulator += std::min(frameTime, maxSpfTime);
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

void PhysicsEngine::reset() {
    this->gameRunning = false;
    this->accumulator = 0.f;
    this->spawner.reset();
}

void PhysicsEngine::startRecording() {
    this->recording = true;
    this->record = Record{};
}

Record PhysicsEngine::stopRecording() {
    this->recording = false;
    this->record.avgStepTime = this->record.avgStepTime / static_cast<double>(this->record.stepTimes.size());
    Record result = this->record;
    this->record = Record{};
    return result;
}

void PhysicsEngine::pause() { this->gameRunning = false; }
