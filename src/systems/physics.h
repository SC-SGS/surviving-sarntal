#pragma once

#include "../components/particle_state.h"
#include "../components/mountain.h"
#include "flecs.h"

struct PhysicSystems {
    explicit PhysicSystems(flecs::world &world);
};

struct ClosestVertex {
    int index;
    bool has_collided;
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

    /**
     * Given a rock, find the closest vertex of the ground.
     *
     * @param it
     * @param position
     * @param radius
     * @param mountain
     * @return int index, bool has_collided
     */
    ClosestVertex getClosestVertex(flecs::iter it, Position position,
                                   Radius radius, Mountain &mountain);

    /**
     * Find the normal vector of a given vertex
     * Calculate, which of the neighbouring points is closer and compute the
     * normal corresponding to the connection line between these two
     * TODO: circular array -> mod size
     * TODO: add noise for later
     * @return normal_vector_x, normal_vector_y
     */
    Position getNormal(std::size_t idx, Position rock_pos, Mountain &m);

    /**
     * Reset the rock to be outside of the terrain and reflect the velocity
     */
    void terrainCollision();

    /**
     * TODO
     */
    void rockCollision();
};
