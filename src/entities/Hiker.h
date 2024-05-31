//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_HIKER_H
#define SURVIVING_SARNTAL_HIKER_H

#include "../components/vector.h"
#include "../output/graphics/render_information/RenderInformation.h"
#include "HikerMovement.h"
#include "RenderedEntity.h"
#include <list>

/**
 * This struct contains information about a collision of a hiker with a rock.
 */
struct HitInformation {
    float radiusRock;
    float velocityRock;
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
    float getHeight() const;
    void setHeight(float height);

    float getWidth() const;
    void setWidth(float width);

    int getHealthPoints() const;
    void setHealthPoints(int healthPoints);

    HikerMovement getHikerMovement() const;
    void setHikerMovement(const HikerMovement &movement);

    bool getIsHit() const;
    void setIsHit(bool isHit);

    HitInformation getHitInformation() const;
    void setHitInformation(const HitInformation &hit);

    Vector getVelocity() const;
    void setVelocity(const Vector &newVel);

    bool getIsAlive() const;
    void setIsAlive(bool alive);

    RenderInformation getRenderInformation() override;

  private:
    Vector velocity{};
    float height{};
    float width{};
    int healthPoints{};
    HikerMovement hikerMovement;
    bool isHit{};
    HitInformation hitInformation{};
    bool isAlive{};
};

#endif // SURVIVING_SARNTAL_HIKER_H
