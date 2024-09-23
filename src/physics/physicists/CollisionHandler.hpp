//
// Created by bjoern on 5/21/24.
//

#ifndef COLLISIONHANDLER_H
#define COLLISIONHANDLER_H

#include "../../entities/World.h"
#include "../../output/graphics/Renderer.h"
#include "../../output/haptics/HapticsService.hpp"
#include "../../utilities/Singleton.hpp"
#include "CollisionDetector.hpp"

/**
 * todo will be changed with mountain
 */
struct Vertex {
    std::size_t index;
    floatType distance;
};

class CollisionHandler {

  public:
    void handleCollisions();

    void setDeltaT(floatType deltaT);
    CollisionHandler(World &world,
                     CollisionDetector &collisionDetector,
                     AudioService &audioService,
                     Renderer &renderer,
                     GameConstants &gameConstants);
    ~CollisionHandler() = default;

  private:
    // Dependencies
    World &world;
    CollisionDetector &collisionDetector;
    AudioService &audioService;
    Renderer &renderer;
    GameConstants &gameConstants;

    floatType deltaT;
    HapticsService &hapticsService;

    /**
     * Tests whether a player collides with the rock and updates his state accordingly.
     * The rock is exploded on impact.
     * TODO nice particle simulation on impact
     *
     * @return
     */
    void playerCollisions() const;

    /**
     * Calculates the damage a collided rock does to the hiker, dependent on its velocity, size and
     * TODO later also the difficulty/type of rock
     *
     * TODO check formula
     *
     * @param rock
     * @return
     */
    int rockDamage(Rock &rock) const;

    /**
     * Checks for all rocks whether they WOULD collide with the terrain in the next step and handles the collision.
     * TODO this is bad and will be changed with spline mountain; also needs lookahead and some analytic methods
     */
    void rockTerrainCollisions(); // const;

    /**
     * This prevents the rock from going into the mountain by reflecting him on the surface.
     * TODO Careful, right now, it leaves a gap (collides early) and skips the time it takes for the rock to hit the
     * TODO terrain and bounce up again
     *
     * TODO Will be changed anyways if we do polygons
     * @param rock
     * @param closestVertex
     */
    void rockTerrainCollision(Rock &rock, Intersection &intersection) const;

    /**
     * Yikes, this is naive.
     *
     * TODO make better with linked cell or verlet neighbor
     *
     */
    void rockRockCollisions();

    /**
     * Collides two rocks.
     *
     * @param rock1
     * @param rock2
     */
    void rockRockCollision(Rock &rock1, Rock &rock2);

    /**
     * Returns the next position of a given rock with a velocity and angular velocity, disregarding any collisions that
     * might occur. This method serves as a preview of the next rock position in a vacuum and should only be used if
     * collisions were handled beforehand or immediately after.
     * TODO check whether positions should be updated before or after collisions or if they are updated in the positions
     * TODO maybe the collision handler and positioner should be melted into one
     * TODO maybe this could be a method of Dwayne Johnson
     *
     * @param rock
     * @return
     */
    Rock getNextState(Rock &rock) const;

    /**
     * Sigh ...
     *
     * @param angVel
     * @return
     */
    floatType capAngularVelocity(floatType angVel) const;
};

#endif // COLLISIONHANDLER_H
