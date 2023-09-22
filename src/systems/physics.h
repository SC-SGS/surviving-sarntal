#pragma once

#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "../components/player.h"
#include "../components/vector.h"
#include "../components/input.h"
#include "flecs.h"

struct PhysicSystems {
    explicit PhysicSystems(flecs::world &world);
};

namespace physics {
constexpr float_type GRAVITATIONAL_CONSTANT = -1000.;
constexpr float_type EPSILON = 1e-3;

struct Vertex {
    std::size_t index;
    float distance;
};

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
void makeRock(const flecs::world &world, Position p, Velocity v,
              float_type radius);

/**
 * Updates velocity and position of a rock.
 *
 * @param it
 * @param positions
 * @param velocities
 */
void updateRockState(flecs::iter it, Position *positions, Velocity *velocities);

/**
 * Reset the rock to be outside of the terrain and reflect the velocity
 */
void terrainCollision(flecs::iter it, Position *positions, Velocity *velocities,
                      Radius *r, Mountain *m);

/**
 * Updates velocity of a rock.
 * //TODO so far, only force considered is gravity
 *
 * @param it
 * @param velocities
 */
void updateRockVelocity(flecs::iter it, Velocity *velocities);

/**
 * Updates position of a rock: pos_new = pos_old + v*dt.
 * Assumes that velocities have been updated beforehand.
 *
 * @param it
 * @param positions
 * @param velocities
 */
void updateRockPosition(flecs::iter it, Position *positions,
                        Velocity *velocities);

/**
 * Given a rock, find the closest vertex of the ground.
 *
 * @param it
 * @param position
 * @param radius
 * @param mountain
 * @return int index, float distance
 */
Vertex getClosestVertex(Position position, Radius radius, Mountain *mountain);

/**
 * Find the normal vector of a given vertex
 * Calculate, which of the neighbouring points is closer and compute the
 * normal corresponding to the connection line between these two
 * TODO: circular array -> mod size
 * TODO: add noise for later
 * @return normal_vector_x, normal_vector_y
 */
Vector getNormal(std::size_t idx, Position rock_pos, Mountain *m);

/**
 * reflect the incident velocity when bouncing off the terrain
 * @param velocity
 * @param normal_vector
 * @return exit_velocity
 */

/**
 * TODO: collision detection is already handled?
 * TODO: should this be public?
 * takes two rocks and updates their velocities and positions
 * after colliding
 * @param p1
 * @param p2
 * @param v1
 * @param v2
 * @param r1
 * @param r2
 */
void rockCollision(Position &p1, Position &p2, Velocity &v1, Velocity &v2,
                   Radius r1, Radius r2);

void quickAndDirtyTest(Position &p1, Position &p2, Velocity &v1, Velocity &v2,
                       Radius r1, Radius r2);

bool isCollided(Position p1, Position p2, Radius r1, Radius r2);

void rockRockInteractions(flecs::iter it, Position *positions,
                          Velocity *velocities, Radius *radius);

/**
 * Updates the state of a player by first updating velocity based on input
 * and current state and afterwards changing position according to velocity in
 * one time step.
 *
 * @param it
 * @param positions
 * @param velocities
 * @param player_movements
 * @param input_entities
 */
void updatePlayerState(flecs::iter it, Position *positions,
                       Velocity *velocities, PlayerMovement *player_movements,
                       InputEntity *input_entities);

/**
 * Returns the exact y coordinate of the mountain at a given x position
 * through linear interpolation.
 *
 * @param world
 * @param x
 * @return the y coordinate
 */
float getYPosFromX(const flecs::world &world, float x);

} // namespace physics
