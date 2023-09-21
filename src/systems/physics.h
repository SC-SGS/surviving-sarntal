#pragma once

#include "../components/particle_state.h"
#include "flecs.h"

struct PhysicSystems {
    explicit PhysicSystems(flecs::world &world);
};

constexpr float GRAVITATIONAL_CONSTANT = 1000.8;

class Collisions {};

class RockTools {

  public:
    /**
     * Create new circular rock, register the new entity with the world,
     * and initialize it with given component values.
     *
     * @param world
     * @param p position
     * @param v velocity
     * @param r radius, should be several times larger than section width in
     * mountain.h
     */
    static void makeRock(const flecs::world &world, Position p, Velocity v,
                         Radius r);

    /**
     * Updates velocity and position of a rock.
     *
     * @param it
     * @param positions
     * @param velocities
     */
    static void updateState(flecs::iter it, Position *positions,
                            Velocity *velocities);

  private:
    /**
     * Updates velocity of a rock.
     * //TODO so far, only force considered is gravity
     *
     * @param it
     * @param positions
     * @param velocities
     */
    static void updateVelocity(flecs::iter it, Position *positions,
                               Velocity *velocities);

    /**
     * Updates position of a rock: pos_new = pos_old + v*dt.
     * Assumes that velocities have been updated beforehand.
     *
     * @param it
     * @param positions
     * @param velocities
     */
    static void updatePosition(flecs::iter it, Position *positions,
                               Velocity *velocities);
};
