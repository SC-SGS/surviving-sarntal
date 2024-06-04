//
// Created by bjoern on 5/21/24.
//

#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"

// TODO so far this is a collection of evil utils, pls come pick me up, I'm scared
class CollisionDetector : public Singleton<CollisionDetector> {
    friend class Singleton<CollisionDetector>;

  public:
    CollisionDetector(const CollisionDetector &) = delete;
    void operator=(const CollisionDetector &) = delete;

    /**
     * TODO This should later return a collision object with a list of collided rock IDs, player is hit and damage to
     * TODO player or be united with the handler
     * TODO or a list of collision objects or something similar
     */
    void detectCollisions() const;

    // TODO later one collides method that can calculate whether arbritrary objects collide? Smaller ball of mud...
    // TODO this method should then also be tested well

    /**
     * Checks whether two given circular rocks collide.
     *
     * TODO maybe later adapt to non-circular rocks and it should be used in some method that uses linked cell or
     * TODO something similar
     *
     * @param rock1
     * @param r2
     * @return
     */
    static bool rocksCollide(RockClass &rock1, RockClass &rock2);

    /**
     * Checks whether the player is hit by a given rock, where the player is represented as a axis-aligned rectangle and
     * the rock as a circle.
     * See
     * <a href="https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection">this</a>.
     *
     * TODO I am not quite happy with this. there should be one method to detect all collisions and return a collObj
     * TODO this method should be tested well
     *
     * @return
     */
    bool isPlayerHitByRock(RockClass &rock);

    /**
     * Used to calculate the normal... AT A VERTEX!!!
     * Only useful because we still have this terrible implementation, where the rocks don't collide with the lines of
     * the mountain, but at the vertices, which are NON-DIFFERENTIABLE  AGHHH.
     * Like... look at this documentation from legacy:
     * Find the normal vector of a given vertex
     * Calculate, which of the neighbouring points is closer and compute the
     * normal corresponding to the connection line between these two
     *
     * TODO this is evil and has to go
     *
     * @param idx
     * @param rockPos
     * @return
     */
    Vector getNormal(std::size_t idx, Vector rockPos) const;

  private:
    World &world;
    CollisionDetector();
    ~CollisionDetector();
};

#endif // COLLISIONDETECTOR_H
