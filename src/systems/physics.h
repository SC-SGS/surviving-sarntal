#pragma once

#include "../components/input.h"
#include "../components/mountain.h"
#include "../components/particle_state.h"
#include "../components/player.h"
#include "../components/vector.h"
#include "flecs.h"

struct PhysicSystems {
    explicit PhysicSystems(flecs::world &world);
};

namespace physics {

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
 * TODO spawn here probably???
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
 * Checks whether the rocks are in scope and deletes those that are not.
 * @param it
 * @param positions
 */
void checkRockInScope(flecs::iter it, Position *positions);

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
                   Radius r1, Radius r2, float dt);

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
                       InputEntity *input_entities, Height *heights,
                       Width *widths);

/**
 * Updates Player's Position based on velocity and current state.
 * Assumes that updatePlayerVelocity has been called beforehand.
 *
 * @param it
 * @param positions
 * @param velocities
 * @param player_movements
 */
void updatePlayerPosition(flecs::iter it, Position *positions,
                          Velocity *velocities,
                          PlayerMovement *player_movements);

/**
 * Updates player velocity based on current input and state.
 *
 * @param it
 * @param positions
 * @param velocities
 * @param player_movements
 * @param input_entities
 */
void updatePlayerVelocity(flecs::iter it, Position *positions,
                          Velocity *velocities,
                          PlayerMovement *player_movements,
                          InputEntity *input_entities, Height *heights);

/**
 * Checks whether jump has been pressed and whether the player is able to jump
 * (double jump within 1.5s is possible).
 * Changes velocity and state accordingly.
 *
 * @param velocities
 * @param player_movements
 * @param input_entities
 */
void checkJumpEvent(Velocity *velocities, PlayerMovement *player_movements,
                    InputEntity *input_entities);

/**
 * Checks whether the duck key is pressed and changes speed and state
 * accordingly. Hiker is slower and
 * TODO hitbox is smaller.
 *
 * @param velocities
 * @param player_movements
 * @param input_entities
 */
void checkDuckEvent(Velocity *velocities, PlayerMovement *player_movements,
                    InputEntity *input_entities, Height *heights);

/**
 * Changes the horizontal speed of the hiker based on input.
 *
 * @param velocities
 * @param player_movements
 * @param input_entities
 */
void checkXMovement(Velocity *velocities, PlayerMovement *player_movements,
                    InputEntity *input_entities);

/**
 * Checks whether the player is in the air and tracks the time he has been
 * there. Applies gravitational force to hiker.
 *
 * @param it
 * @param velocities
 * @param player_movements
 * @param input_entities
 */
void checkAerialState(flecs::iter it, Velocity *velocities,
                      PlayerMovement *player_movements,
                      InputEntity *input_entities);

/**
 * Changes the state of the player according to the direction of his horizontal
 * movement. Mostly needed for visualization purposes.
 *
 * @param velocities
 * @param player_movements
 * @param input_entities
 */
void checkDirection(Velocity *velocities, PlayerMovement *player_movements,
                    InputEntity *input_entities);

/**
 * Checks whether a rock hits the player, making him unalive and stopping the
 * game.
 *
 * @param it
 * @param positions of the player
 * @param heights of the player
 * @param widths of the player
 */
void checkPlayerIsHit(flecs::iter it, Position *positions, Radius *radii);

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
