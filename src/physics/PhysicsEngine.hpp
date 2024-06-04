//
// Created by bjoern on 5/20/24.
//

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "../entities/World.h"
#include "../input/events/GameEvent.h"
#include "../utilities/Singleton.hpp"
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
 */
class PhysicsEngine : public Singleton<PhysicsEngine> {
    friend class Singleton<PhysicsEngine>;

  public:
    /**
     * Perform N = frameTime / deltaT update steps and interpolate at the end.
     * The first of the N updates takes into consideration the inputs from the last iteration of the game loop.
     * //TODO is it possible to interpolate the state with no return value
     * The state of the world at the end of this method should be the state at exactly the time, the next frame is
     * rendered.
     *
     * This method accepts a compromise between having a fixed time step and caclculating the exact state of the game
     * at render time. We accept having a latency of up to min(frameTime, deltaT) and corresponding input aliasing
     * effects.
     * //TODO only way I see how to solve this would be having physics on different thread but I believe we should try
     * // this version first and check later how it looks and feels
     * // This is a general problem: Do the physics run slower than the frame rate or can we split the physics in
     * // parts that are tied to frame rate (input) and parts that are not (particles/rocks)
     * // do we need to change input? Is input coupled to frame rate?
     *
     * @param events
     */
    void update(std::queue<GameEvent> &events);
    float getDeltaT() const;
    void setDeltaT(float deltaT);

  private:
    /**
     * The constant length of a simulation time interval.
     */
    float deltaT;

    /**
     * The world containing all entities.
     */
    World &world;

    /**
     * Timestamp of last update.
     */
    float timeLastUpdate;

    /**
     * Time produced by the renderer that is consumed by the simulation in constant and discrete deltaT sized chunks.
     */
    float accumulator;

    /**
     * Responsible for spawning entities at the start of each step.
     */
    const Spawner &spawner;

    /**
     * The event processor, changing the world state based on a given list of Input events.
     */
    EventProcessor &eventProcessor;

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
     * The private constructor method of the PhysicsEngine.
     * Only to be called once.
     */
    PhysicsEngine();

    /**
     * Simulates the change in the state of the world over the next deltaT time interval.
     */
    void updateTimeStep() const;

    /**
     * Destructs all physicists
     * and sets isInit to false;
     */
    ~PhysicsEngine();
};

#endif // PHYSICSENGINE_H
