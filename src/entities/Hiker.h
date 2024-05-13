//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_HIKER_H
#define SURVIVING_SARNTAL_HIKER_H

#include "../components/vector.h"
#include "HikerMovement.h"
#include "RenderInformation.h"
#include "RenderedEntity.h"

struct HitInformation {
    float radius_rock;
    float velocity_rock;
    int counting_variable;
};

/**
 * This class represents the player in the game which is controlled by the user.
 * The hiker has a position as well as velocity and stores information about its
 * movement, collision with rocks (in case a collision happened) as well as the
 * health points.
 */
class Hiker : public RenderedEntity {

  public:
    Hiker();

    Hiker(HikerMovement hikerMovement, Vector velocity);

    float getHeight() const;
    void setHeight(float h);

    float getWidth() const;
    void setWidth(float w);

    int getHealthPoints() const;
    void setHealthPoints(int hp);

    HikerMovement getHikerMovement() const;
    void setHikerMovement(const HikerMovement &movement);

    bool getIsHit() const;
    void setIsHit(bool isHitted);

    HitInformation getHitInformation() const;
    void setHitInformation(const HitInformation &hit);

    Vector getVelocity() const;
    void setVelocity(const Vector &newVel);

    bool getIsAlive() const;
    void setIsAlive(bool alive);

  private:
    Vector velocity;
    float height;
    float width;
    int healthPoints;
    HikerMovement hikerMovement;
    bool isHit;
    HitInformation hitInformation;
    bool isAlive;
};

#endif // SURVIVING_SARNTAL_HIKER_H
