//
// Created by bjoern on 5/20/24.
//

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "../input/events/GameEvent.h"
#include "physicists/Accelerator.hpp"
#include "physicists/CollisionDetector.hpp"
#include "physicists/CollisionHandler.hpp"
#include "physicists/Destructor.hpp"
#include "physicists/GameEventProcessor.hpp"
#include "physicists/Interpolator.hpp"
#include "physicists/Positioner.hpp"
#include "physicists/Spawner.hpp"

#include <list>

/**
 * A record of time step sizes.
 */
struct Record {
    /**
     * Average time needed for a physics step.
     */
    double avgStepTime{0};

    /**
     * Precise list of times needed for physics steps.
     */
    std::list<std::tuple<double, double>> stepTimes{};
};

class Recorder;

/**
 * <H1>This is the Physics Engine of our game.</H1>
 *
 * <p>It performs the update of the world state N times per frame, where each step of the simulation simulates a <b>time
 * interval of constant length</b> deltaT. See also
 * <a href="https://web.archive.org/web/20130328024036/http://gafferongames.com/game-physics/fix-your-timestep/">Fix
 * your Timestep</a>.</p>
 */
class PhysicsEngine {

  public:
    /**
     * The exponential moving average of the time needed for a physics step.
     */
    static double exponentialMovingAvg;

    /**
     * @brief Performs the physics update, simulating the time for the last frame.
     *
     * Perform N = frameTime / deltaT update steps and calculateYPos at the end.
     * The first of the N updates takes into consideration the inputs from the last iteration of the game loop.
     * The state of the world at the end of this method should be the state at exactly the time, the next frame is
     * rendered.
     *
     * This method accepts a compromise between having a fixed time step and calculating the exact state of the game
     * at render time. We accept having a latency of up to min(frameTime, deltaT) and corresponding input aliasing
     * effects.
     *
     * @param events
     */
    void update(std::queue<GameEvent> &events);
    floatType getDeltaT() const;
    void setDeltaT(floatType deltaT);

    /**
     * @brief Resets the physics engine
     */
    void reset();

    /**
     * @brief Starts recording the time needed for update steps.
     */
    void startRecording();

    /**
     * @brief Stops the current recording and returns it.
     *
     * Should only be called after a recording has been started if you want meaningful results.
     * @return
     */
    Record stopRecording();

    PhysicsEngine(World &world,
                  Spawner &spawner,
                  PhysicsConstants &physicsConstants,
                  GameEventProcessor &eventProcessor,
                  Accelerator &accelerator,
                  Positioner &positioner,
                  CollisionDetector &collisionDetector,
                  CollisionHandler &collisionHandler,
                  Interpolator &interpolator,
                  Destructor &destructor);

    ~PhysicsEngine() = default;

    /**
     * @brief Pauses the physics engine until the next call of update()
     *
     * Sets gameRunning to false, making sure, that the next physics step restarts the timer.
     */
    void pause();

  private:
    /**
     * The constant length of a simulation time interval.
     */
    floatType deltaT;

    /**
     * Checks if the game is running. Needed because we use GetTime.
     */
    bool gameRunning{false};

    /**
     * The world containing all entities.
     */
    World &world;

    /**
     * The physics constants.
     */
    PhysicsConstants &physicsConstants;

    /**
     * Timestamp of last update.
     */
    floatType timeLastUpdate;

    /**
     * Time produced by the renderer that is consumed by the simulation in constant and discrete deltaT sized chunks.
     */
    floatType accumulator;

    /**
     * Responsible for spawning entities at the start of each step.
     */
    Spawner &spawner;

    /**
     * The event processor, changing the world state based on a given list of Input events.
     */
    GameEventProcessor &eventProcessor;

    /**
     * The force calculator, calculating the forces and accelerations of every movable entity.
     */
    Accelerator &accelerator;

    /**
     * The positioner, updating the position of hiker and rocks based on their current positions, verlocities and
     * accelerations.
     */
    Positioner &positioner;

    /**
     * The collision detector, checking for collisions of objects after an update step by the positioner.
     */
    CollisionDetector &collisionDetector;

    /**
     * If collisions have been detected, they need to be resolved by the collision handler.
     */
    CollisionHandler &collisionHandler;

    /**
     * Interpolates the position of objects at rendering time and saves it in the render info.
     */
    const Interpolator &interpolator;

    /**
     * Responsible for destructing all entities that are outside of the world borders.
     */
    const Destructor &destructor;

    /**
     * A record of time step sizes.
     */
    Record record;

    /**
     * Can be toggled to start and stop recording the times needed for physics steps.
     */
    bool recording{false};

    /**
     * Simulates the change in the state of the world over the next deltaT time interval.
     */
    void updateTimeStep();

    void startEngine();

    void updateAccumulator();

    void updateTimeStepAndRecord();

    void recordTimeStep(double t0Start, double t1Stop);
};

#endif // PHYSICSENGINE_H
