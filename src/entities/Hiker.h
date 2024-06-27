//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_HIKER_H
#define SURVIVING_SARNTAL_HIKER_H

#include "../output/audio/AudioService.hpp"
#include "../output/graphics/renderInformation/RenderInformation.h"
#include "../utilities/vector.h"
#include "HikerMovement.h"
#include "RenderedEntity.h"
#include <list>

/**
 * This struct contains information about a collision of a hiker with a rock.
 */
struct HitInformation {
    floatType radiusRock;
    floatType velocityRock;
    int countingVariable;
};

/**
 * This class represents the player in the game which is controlled by the user.
 * The hiker has a position as well as velocity and stores information about its
 * movement, collision with rocks (in case a collision happened) as well as the
 * health points.
 */
class Hiker : public RenderedEntity {

  public:
    explicit Hiker(Vector position);
    floatType getHeight() const;
    void setHeight(floatType height);

    floatType getWidth() const;
    void setWidth(floatType width);

    int getHealthPoints() const;
    void setHealthPoints(int healthPoints);

    const HikerMovement &getHikerMovement() const;
    void setHikerMovement(const HikerMovement &movement);

    bool getIsHit() const;
    void setIsHit(bool isHit);

    const HitInformation &getHitInformation() const;
    void setHitInformation(const HitInformation &hit);

    const Vector &getVelocity() const;
    void setVelocity(const Vector &newVel);

    bool getIsAlive() const;
    void setIsAlive(bool alive);

    RenderInformation getRenderInformation() override;

    void turnLeft();
    void turnRight();
    void turnNeutral();
    void setLastJump(float lastJump);

    void crouch();
    void uncrouch();
    void jump();
    void setHikerMoving();

    void moveToRight(floatType deltaX);
    void moveToLeft(floatType deltaX);

    void accelerateX(floatType deltaX);
    void accelerateY(floatType deltaY);
    void setXVelocity(floatType xValue);
    void setYVelocity(floatType yValue);

    void addHealthPoints(int points);

    bool needsToDie() const;
    void kill();

  private:
    // AudioService &audioService = AudioService::getInstance();
    Vector velocity{};
    floatType height{};
    floatType width{};
    int healthPoints{};
    HikerMovement hikerMovement{};
    bool isHit{};
    HitInformation hitInformation{};
    bool isAlive{};
    void doSecondJump();
};

#endif // SURVIVING_SARNTAL_HIKER_H
