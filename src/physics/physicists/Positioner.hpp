//
// Created by bjoern on 5/21/24.
//

#ifndef POSITIONER_H
#define POSITIONER_H

#include "../../entities/World.h"

class Positioner {

  public:
    explicit Positioner(World &world,
                        HikerConstants &hikerConstants,
                        BarriersConstants &barriersConstants,
                        PhysicsConstants &physicsConstants,
                        TerrainConstants &terrainConstants);
    ~Positioner() = default;

    /**
     * @brief Updates position of world entities.
     *
     * The rocks and the hiker are moved to the next position that is reached in the discrete time step with their
     * current velocity. May violate rigid body constraints and lead to an invalid world state if collisions are not
     * handled afterwards. The world border and monster are moved in x direction. The monster is placed on top of the
     * terrain.
     */
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
     * Applies all knockbacks to the hiker that need to be applied according to the hitInformation of the hiker.
     *
     */
    void applyKnockbackToHiker() const;

    /**
     * Updates the hiker position if they are moving on the terrain and changes the state to in air, if they fall of the
     * terrain in this step.
     *
     * Should only be called if the hiker is currently on the terrain.
     */
    void moveHikerOnTerrain() const;

    /**
     * Calculate the theoretical next hiker position based on the velocity of the hiker.
     *
     * @return
     */
    Vector calculateTheoreticalNextHikerPosition(floatType speedFactor) const;

    /**
     * Moves the monster forward so that it always aligns with the left screen border.
     */
    void updateMonsterPosition() const;

    /**
     * Moves the left and right world border at each step to match the currently displayed part of the world.
     */
    void updateWorldBorderPosition() const;
};

#endif // POSITIONER_H
