//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCK_H
#define SURVIVING_SARNTAL_ROCK_H

#include "../utilities/vector.h"

#include "RenderedEntity.h"

/**
 * This class represents a rock in the game. A rock is simulated by a certain point (position) with a radius
 * and the attributes velocity and rotation.
 */
// TODO struct might be better for performance?
// TODO what about destructors, as in the programming concept in c++?
class Rock : public RenderedEntity {
  public:
    Rock(Vector position, Vector velocity, floatType angularVelocity, floatType angularOffset, float radius);
    ~Rock() = default;

    void setVelocity(Vector &newVelocity);
    void setAngularVelocity(floatType newAngularVelocity);
    void setAngularOffset(floatType newAngularOffset);

    Vector getVelocity();
    floatType getAngularVelocity() const;
    floatType getAngularOffset() const;
    floatType getRadius() const;

    RenderInformation getRenderInformation() const override;

    bool getShouldBeDestroyed() const;
    void setShouldBeDestroyed(bool shouldBeDestroyed);

  private:
    Vector velocity{};
    floatType angularVelocity;
    floatType angularOffset;
    floatType radius = MIN_ROCK_SIZE;
    bool shouldBeDestroyed = false;
};

#endif // SURVIVING_SARNTAL_ROCK_H