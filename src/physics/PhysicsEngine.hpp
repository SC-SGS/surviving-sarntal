//
// Created by bjoern on 5/20/24.
//

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "../entities/World.h"
#include "../input/events/GameEvent.h"
#include "physicists/Accelerator.hpp"
#include "physicists/CollisionDetector.hpp"
#include "physicists/CollisionHandler.hpp"
#include "physicists/Destructor.hpp"
#include "physicists/EventProcessor.hpp"
#include "physicists/Interpolator.hpp"
#include "physicists/Positioner.hpp"
#include "physicists/Spawner.hpp"

#include <list>

/**
 * <H1>This is the Physics Engine of our game.</H1>
 *
 * <p>It performs the update of the world state N times per frame, where each step of the simulation simulates a <b>time
 * interval of constant length</b> deltaT. See also
 * <a href="https://web.archive.org/web/20130328024036/http://gafferongames.com/game-physics/fix-your-timestep/">Fix
 * your Timestep</a>.</p>
 *
 * <p>The Physics Engine follows the <b>Singleton</b> design pattern as there can only be one.
 * However, this implementation of the Singleton design pattern is not Thread safe, so the PhysicsEngine should not be
 * created in parallel threads.
 * We accept this implementation, because the PhysicsEngine will be created in the game loop or before, where no
 * concurrency is intended.</p>
 */
class PhysicsEngine {

  public:
    // TODO there should be a destructor here somewhere, destructing all the physicists
    /**
     * Singletons should not be cloneable.
     */
    PhysicsEngine(PhysicsEngine &other) = delete;

    /**
     * Singletons should not be assignable.
     */
    void operator=(const PhysicsEngine &) = delete;

    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates the PhysicsEngine object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     *
     * @param deltaT the constant time step size for the physics simulation
     * @param world the world
     */
    static PhysicsEngine *getPhysicsEngine(float deltaT, World &world);

    /**
     * Perform N = frameTime / deltaT update steps and interpolate at the end.
     * The first of the N updates takes into consideration the inputs from the last iteration of the game loop.
     * //TODO is it possible to intepolate the state with no return value
     * The state of the world at the end of this method should be the state at exactly the time, the next frame is
     * rendered.
     *
     * This method accepts a compromise between having a fixed time step and caclculating the exact state of the game
     * at render time. We accept having a latency of up to min(frameTime, deltaT) and corresponding input aliasing
     * effects.
     * //TODO only way I see how to solve this would be having physics on different thread but I believe we should try
     * //TODO this version first and check later how it looks and feels
     * //TODO This is a general problem: Do the physics run slower than the frame rate or can we split the physics in
     * //TODO parts that are tied to frame rate (input) and parts that are not (particles/rocks)
     * //TODO do we need to change input? Is input coupled to frame rate?
     *
     * // TODO NO, events have to be gotten in every step!!!
     *
     * @param frameTime
     */
    void update(float frameTime);

  private:
    /**
     * The singleton object or null, if not yet initialized
     */
    static PhysicsEngine *physicsEngine;

    /**
     * The constant length of a simulation time interval.
     */
    const float deltaT;

    /**
     * The world containing all entities.
     */
    World &world;

    /**
     * Time produced by the renderer that is consumed by the simulation in constant and discrete deltaT sized chunks.
     */
    float accumulator{};

    /**
     * Responsible for spawning entities at the start of each step.
     */
    const Spawner *spawner;

    /**
     * The InputHandler responsible for processing all inputs.
     */
    const InputHandler *inputHandler;

    /**
     * The event processor, changing the world state based on a given list of Input events.
     */
    EventProcessor *eventProcessor;

    /**
     * The force calculator, calculating the forces and accelerations of every movable entity.
     */
    const Accelerator *accelerator;

    /**
     * The positioner, updating the position of hiker and rocks based on their current positions, verlocities and
     * accelerations.
     */
    const Positioner *positioner;

    /**
     * The collision detector, checking for collisions of objects after an update step by the positioner.
     */
    CollisionDetector *collisionDetector;

    /**
     * If collisions have been detected, they need to be resolved by the collision handler.
     */
    CollisionHandler *collisionHandler;

    /**
     * Interpolates the position of objects at rendering time and saves it in the render info.
     */
    const Interpolator *interpolator;

    /**
     * Responsible for destructing all entities that are outside of the world borders.
     */
    const Destructor *destructor;

    /**
     * The private constructor method of the PhysicsEngine.
     * Only to be called once.
     *
     * @param deltaT simulation time step size
     * @param world the world
     */
    PhysicsEngine(float deltaT, World &world);

    /**
     * Simulates the change in the state of the world over the next deltaT time interval.
     */
    void updateTimeStep() const;
};

#endif // PHYSICSENGINE_H
