#pragma once

#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "flecs.h"

struct PhysicSystems {
    explicit PhysicSystems(flecs::world &world);
};

struct Vertex {
    std::size_t index;
    float distance;
};

constexpr float GRAVITATIONAL_CONSTANT = 1000.8;

constexpr float JUMP_VELOCITY_CONSTANT = 10000;

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

    class TerrainCollisions {

        /**
         * Given a rock, find the closest vertex of the ground.
         *
         * @param it
         * @param position
         * @param radius
         * @param mountain
         * @return int index, float distance
         */
        static Vertex getClosestVertex(flecs::iter it, Position position,
                                       Radius radius, Mountain &mountain);

        /**
         * Find the normal vector of a given vertex
         * Calculate, which of the neighbouring points is closer and compute the
         * normal corresponding to the connection line between these two
         * TODO: circular array -> mod size
         * TODO: add noise for later
         * @return normal_vector_x, normal_vector_y
         */
        static Position getNormal(std::size_t idx, Position rock_pos,
                                  Mountain &m);

        /**
         * Reset the rock to be outside of the terrain and reflect the velocity
         */
        static void terrainCollision();
    };

    class RockCollisions {
      public:
        /**
         * Let all colliding Rocks collide and update their positions and
         * velocities. (TODO necessary?)
         */
        static void rockCollisions(flecs::world &world);

      private:
        /**
         * Let a pair of rocks collide and update their positions and
         * velocities.
         */
        static void rockCollision(flecs::world &world);

        /**
         * Adds a "CollidesWith" pair to two rocks if they collide
         */
        static void pairCollidingRocks(flecs::world &world);

        /**
         * Adds pair "PossiblyCollidesWith" to pairs of possibly colliding
         * rocks.
         */
        static void pairPossiblyCollidingRocks(flecs::iter it,
                                               Position *positions,
                                               Radius *radii);
    };
};

class PlayerTools {
  public:
    static void updateState(flecs::iter it, Position *positions,
                     Velocity *velocities);
};
