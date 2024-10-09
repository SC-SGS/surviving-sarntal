//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_HIKER_H
#define SURVIVING_SARNTAL_HIKER_H
#pragma once

#include "../geometry/DynamicConvexPolygon.h"
#include "../output/audio/AudioService.hpp"
#include "../output/graphics/renderInformation/RenderInformation.h"
#include "../terrain/points/StaticPolygon.hpp"
#include "../terrain/spatialDatastructure/AxisAlignedBoundingBox.hpp"
#include "../utilities/vector.h"
#include "HikerMovement.h"
#include "RenderedEntity.h"
#include <list>

class World;

/**
 * This struct contains information about a collision of a hiker with a rock.
 */
struct HitInformation {
    Vector knockback;
    int maxSteps;
    int currentSteps;
};

/**
 * This class represents the player in the game which is controlled by the user.
 * The hiker has a position as well as velocity and stores information about its
 * movement, collision with rocks (in case a collision happened) as well as the
 * health points.
 */
class Hiker : public RenderedEntity {

  public:
    explicit Hiker(Vector position, AudioService &audioService, HikerConstants hikerConstants);
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

    const std::vector<HitInformation> &getHitInformation() const;
    void setHitInformation(const std::vector<HitInformation> &hits);
    void addHitInformation(const HitInformation &hit);

    const Vector &getVelocity() const;
    void setVelocity(const Vector &newVel);

    bool getIsAlive() const;
    void setIsAlive(bool alive);

    const Vector &getKnockback() const;
    void setKnockback(const Vector &newKnockback);

    RenderInformation getRenderInformation() const override;

    void turnLeft();
    void turnRight();
    void turnNeutral();
    void setLastJump(float lastJump);

    void crouch();
    void uncrouch();
    void jump();
    void setMoving();
    void setInAir();

    void moveToRight(floatType deltaX);
    void moveToLeft(floatType deltaX);

    void accelerateX(floatType deltaX);
    void accelerateY(floatType deltaY);
    void setXVelocity(floatType xValue);
    void setYVelocity(floatType yValue);

    void addHealthPoints(int points);
    void doDamagePoints(int damagePoints);

    void kill();
    bool hasShield() const;
    /**
     * Sets shield for the hiker for a given time.
     * @param time the number of seconds will be active for
     */
    void setShield(double time);

    /**
     * Computes the relative multiplier of the hiker speed for the given slope.
     *
     * @param movement
     * @return
     */
    floatType computeSpeedFactor(const Vector &movement) const;

    /**
     * Returns the bounding box for the current position and state of this hiker.
     *
     * @return
     */
    std::shared_ptr<DynamicConvexPolygon> getCurrentHitbox() const;

    void setPosition(const Vector &position) override;
    void move(const Vector &movement);
    void reset(const Vector &position);

    const Vector &getCurrentHitboxDelta() const;

    void resetHitboxAngularMomentum();

    void setShouldUncrouch();
    bool getShouldUncrouch() const;

    std::shared_ptr<DynamicConvexPolygon> getHitboxWalking() const;
    std::shared_ptr<DynamicConvexPolygon> getHitboxCrouched() const;

  private:
    // Dependencies
    AudioService &audioService;
    HikerConstants hikerConstants;

    // Attributes
    Vector velocity{};
    floatType height{};
    floatType width{};
    int healthPoints{};
    HikerMovement hikerMovement{};
    Vector knockback = {0.f, 0.f};

    bool isAlive{};
    std::shared_ptr<DynamicConvexPolygon> hitboxWalking = nullptr;
    std::shared_ptr<DynamicConvexPolygon> hitboxCrouched = nullptr;
    Vector walkingHitboxDelta{};  // Delta from the hiker position to the position of their walking hitbox
    Vector crouchedHitboxDelta{}; // Delta from the hiker position to the position of their crouched hitbox
    floatType
        crouchedToWalkingDelta{}; // delta by which the crouched hitbox is to be placed lower than the walking hitbox
    bool shouldUncrouch = false;
    // std::shared_ptr<DynamicPolygon> boundingBoxCrouched = nullptr;
    // Vector crouchedHitBoxDelta{};
    // std::shared_ptr<DynamicPolygon> boundingBoxJumping = nullptr;
    // Vector jumpingHitBoxDelta{};
    double shieldTime{};

    void setHitboxVelocity(const Vector &velocity);

    void updateDirection();
};

#endif // SURVIVING_SARNTAL_HIKER_H
