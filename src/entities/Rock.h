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
    Rock(Vector position, Vector velocity, Rotation rotation, floatType radius) : RenderedEntity(position) {
        this->position = position;
        this->velocity = velocity;
        this->rotation = rotation;
        this->radius = radius;
    };

    ~Rock() = default;

    void setVelocity(Vector &newVelocity);
    void setRotation(const Rotation &newRotation);

    Vector getVelocity();
    Rotation getRotation();
    floatType getRadius() const;

    RenderInformation getRenderInformation() override;

    bool getShouldBeDestroyed() const;
    void setShouldBeDestroyed(bool shouldBeDestroyed);

  private:
    Vector velocity{};
    Rotation rotation{};
    floatType radius = MIN_ROCK_SIZE;
    bool shouldBeDestroyed = false;
};

#endif // SURVIVING_SARNTAL_ROCK_H