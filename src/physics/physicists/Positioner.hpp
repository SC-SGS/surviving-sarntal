//
// Created by bjoern on 5/21/24.
//

#ifndef POSITIONER_H
#define POSITIONER_H

#include "../../entities/World.h"
#include "../../utilities/Singleton.hpp"

class Positioner {

  public:
    explicit Positioner(World &world,
                        HikerConstants &hikerConstants,
                        BarriersConstants &barriersConstants,
                        PhysicsConstants &physicsConstants,
                        TerrainConstants &terrainConstants);
    ~Positioner() = default;
    void updatePositions() const;

    void setDeltaT(floatType deltaT);

  private:
    World &world;
    floatType deltaT;
    HikerConstants &hikerConstants;
    BarriersConstants &barriersConstants;
    TerrainConstants &terrainConstants;
    PhysicsConstants &physicsConstants;

    /**
     * Use movement equations to update rock positions.
     */
    void updateRockPositions() const;

    /**
     * Use movement equations to update rock positions.
     */
    void updateHikerPosition() const;

    /**
     * Computes the knockback to be applied to the hiker.
     *
     * @return
     */
    Vector getHikerKnockback() const;

    /**
     * Applies all knockbacks to the hiker that need to be applied according to the hitInformation of the hiker.
     *
     */
    void applyKnockbackToHiker() const;

    /**
     * Updates the hiker position if they are in air and changes the state to moving, if they hit the terrain in this
     * step.
     *
     * Should only be called if the hiker is currently in air.
     */
    void moveHikerInAir() const;

    /**
     * Updates the hiker position if they are moving on the terrain and changes the state to in air, if they fall of the
     * terrain in this step.
     *
     * Should only be called if the hiker is currently on the terrain.
     */
    void moveHikerOnTerrain() const;

    /**
     * Restricts the movement of the hiker so that they can't leave the Screen to the right side.
     */
    void enforceWorldLimitOnHiker() const;

    /**
     * Maps the y-position of the hiker to the nearest terrain in y-direction.
     * TODO: Could be the soure of weird glitching to the terrain if called in inappropiate places.
     */
    void clampHikerToTerrain() const;

    /**
     * Checks whether the lowest position of the hiker is near the terrain.
     *
     * @param movement
     * @return true if the hiker feet are near the terrain AND the current movement is downwards.
     */
    bool hikerFeetNearTerrain(Vector movement) const;

    /**
     * Updates the hiker state based on the current hiker position and the given movement of the hiker.
     *
     * For example: If the hiker is near the terrain and currently moving downwards, they will be mapped to the terrain
     * and set in the state moving.
     *
     * @param movement
     */
    void updateHikerState(Vector movement) const;

    /**
     * Moves the hiker by the specifed movement vector, detects and handles collisions if they arise.
     *
     * @param movement
     */
    void moveHiker(const Vector &movement) const;

    /**
     * Calculate the theoretical next hiker position based on the velocity of the hiker.
     *
     * @return
     */
    Vector calculateTheoreticalNextHikerPosition() const;

    /**
     * Moves the monster forward so that it always aligns with the left screen border.
     */
    void updateMonsterPosition() const;

    /**
     * Moves the left and right world border at each step to match the currently displayed part of the world.
     */
    void updateWorldBorderPosition() const;

    /**
     * If the hiker collides with the terrain at its current position, the hiker is placed just outside the terrain.
     *
     * @param movement the last movement of the hiker that placed them in their current position.
     */
    void resolveHikerTerrainCollisions(Vector movement) const;

    /**
     * Computes the speed  based on the covered distance.
     */
    floatType getKillBarVelocity() const;
};

#endif // POSITIONER_H
