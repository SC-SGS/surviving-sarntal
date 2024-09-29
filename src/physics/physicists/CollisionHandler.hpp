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
    void handleCollisions() const;
    void resolveRockCollisions() const;

    void setDeltaT(floatType deltaT);
    CollisionHandler(World &world,
                     const CollisionDetector &collisionDetector,
                     AudioService &audioService,
                     Renderer &renderer,
                     const GameConstants &gameConstants);
    ~CollisionHandler() = default;

  private:
    // Dependencies
    World &world;
    const CollisionDetector &collisionDetector;
    AudioService &audioService;
    Renderer &renderer;
    const GameConstants &gameConstants;

    floatType deltaT;
    HapticsService &hapticsService;

    /**
     * Checks whether a rock has a terrain collision and handles it if necessary by displacing and applying impulses.
     * A simple overlap does not necessarily constitute a collision if the relative velocity is greater than 0.
     * This case usually occurs when this collision has been resolved previously but due to other collsions the rock
     * intersects with the terrain again. If the rock does not move into the terrain, but away from it, we do not have a
     * true collision and no impulses are applied, since there will not be an intersection anymore in a few steps.
     *
     * @param rock
     * @return
     */
    bool checkAndHandleTerrainCollision(Rock &rock) const;

    /**
     * Checks a pair of rocks for collision and handles it if necessary by displacing and applying impulses.
     * A simple overlap does not necessarily constitute a collision if the relative velocity is greater than 0.
     * This case usually occurs when this collision has been resolved previously but due to other collsions the rocks
     * intersect again. If they do not move into each other, we do not have a true collision and no impulses are
     * applied.
     *
     * @param rock
     * @param rock2
     * @return
     */
    bool checkAndHandleRockCollision(Rock &rock, Rock &rock2) const;

    /**
     * Displaces the rock after he collided with the terrain so that he no longer collides with it. In contrast to
     * resolveDynamicPolygonCollisionByDisplacement, the full displacement is applied to the rock as the terrain is
     * obviously static.
     *
     * @param terrainCollision
     * @param eps
     */
    static void
    resolveDynamicPolygonTerrainCollisionByDisplacement(const DynamicPolygonTerrainCollisionObject &terrainCollision,
                                                        floatType eps);

    /**
     * Displaces 2 colliding dynamic polygons so that they no longer collide.
     * The amount of displacement is proportionate to their mass and a small eps is considered to ensure the collision
     * is truly resolved.
     *
     * @param collision
     * @param eps
     */
    static void resolveDynamicPolygonCollisionByDisplacement(const DynamicPolygonCollisionObject &collision,
                                                             floatType eps);

    /**
     * Applies impulses if the rock still moves towards the terrain after the collision has been resolved by
     * displacement.
     *
     * @param terrainCollision
     * @param bounciness
     * @param eps
     */
    static bool applyImpulsesTerrainCollision(const DynamicPolygonTerrainCollisionObject &terrainCollision,
                                              floatType bounciness,
                                              floatType eps);

    /**
     * Applies impulses if the rocks still move towards each other after the collision has been resolved by
     * displacement.
     *
     * @param polygonCollision
     * @param bounciness
     * @param eps
     * @return
     */
    static bool applyImpulsesPolygonCollision(const DynamicPolygonCollisionObject &polygonCollision,
                                              floatType bounciness,
                                              floatType eps);

    /**
     * Calculates the denominator term in the calculation of the magnitude of the impulse for the inc or the ref
     * polygon of the collision.
     *
     * @param poly
     * @param normal
     * @param contactPoint
     * @return
     */
    static floatType
    calculateDenominatorTermFor(const DynamicConvexPolygon &poly, const Vector &normal, const Vector &contactPoint);

    /**
     * Tests whether a player collides with the rock and updates his state accordingly.
     * The rock is exploded on impact.
     * TODO nice particle simulation on impact
     *
     * @return
     */
    void playerCollisions() const;

    void checkAndHandlePlayerRockCollision(Rock &rock) const;

    void resolvePlayerRockCollisions() const;

    void checkAndHandlePlayerTerrainCollisions() const;

    /**
     * Calculates the damage a collided rock does to the hiker, dependent on its velocity, size and
     * TODO later also the difficulty/type of rock
     *
     * TODO check formula
     *
     * @param rock
     * @return
     */
    int rockDamage(const Rock &rock) const;

    /**
     * Calculates the hit information for a rock if this rock was to hit the hiker.
     *
     * @param coll
     * @return
     */
    Vector computeKnockback(const DynamicPolygonCollisionObject &coll) const;

    /**
     * Checks for all rocks whether they WOULD collide with the terrain in the next step and handles the collision.
     * TODO this is bad and will be changed with spline mountain; also needs lookahead and some analytic methods
     *
    void rockTerrainCollisions(); // const;

    /**
     * This prevents the rock from going into the mountain by reflecting him on the surface.
     * TODO Careful, right now, it leaves a gap (collides early) and skips the time it takes for the rock to hit the
     * TODO terrain and bounce up again
     *
     * TODO Will be changed anyways if we do polygons
     * @param rock
     * @param closestVertex
     *
    void rockTerrainCollision(Rock &rock, Intersection &intersection) const;

    /**
     * Yikes, this is naive.
     *
     * TODO make better with linked cell or verlet neighbor
     *
     *
    void rockRockCollisions();

    /**
     * Collides two rocks.
     *
     * @param rock1
     * @param rock2
     *
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
     *
    Rock getNextState(Rock &rock) const;

    /**
     * Sigh ...
     *
     * @param angVel
     * @return
     *
    floatType capAngularVelocity(floatType angVel) const;*/
};

#endif // COLLISIONHANDLER_H
