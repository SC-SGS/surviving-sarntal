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

class CollisionHandler {

  public:
    void handleCollisions() const;
    void resolveRockCollisions() const;

    void setDeltaT(floatType deltaT);
    CollisionHandler(World &world,
                     const CollisionDetector &collisionDetector,
                     AudioService &audioService,
                     InputHandler &inputHandler,
                     Renderer &renderer,
                     const GameConstants &gameConstants);
    ~CollisionHandler() = default;

  private:
    // Dependencies
    World &world;
    const CollisionDetector &collisionDetector;
    AudioService &audioService;
    InputHandler &inputHandler;
    Renderer &renderer;
    const GameConstants &gameConstants;

    floatType deltaT;

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

    /**
     *
     * @return whether the hiker can uncrouch without getting stuck in the terrain
     */
    bool hikerCanUncrouch() const;

    /**
     * If the hiker collides with the terrain at its current position, the hiker is placed just outside the terrain.
     */
    bool resolveHikerTerrainCollision(bool inAir) const;

    /**
     * Restricts the movement of the hiker so that they can't leave the Screen to the right side.
     */
    void enforceWorldLimitOnHiker() const;

    /**
     * Calculates the damage a collided rock does to the hiker, dependent on its velocity and size.
     * TODO dependent on mass / impulse
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
     * Clamps the hiker to the nearest terrain below him if they intersect this terrain.
     */
    void clampHikerIfNeeded() const;
};

#endif // COLLISIONHANDLER_H
