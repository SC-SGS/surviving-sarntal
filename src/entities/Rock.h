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
// TODO struct might be better for performance, is it called RockClass?
// TODO what about destructors, as in the programming concept in c++?
class RockClass : public RenderedEntity {
  public:
    RockClass(Vector position, Vector velocity, Rotation rotation, float radius) : RenderedEntity(position) {
        this->position = position;
        this->velocity = velocity;
        this->rotation = rotation;
        this->radius = radius;
    };

    ~RockClass() = default;

    void setVelocity(Vector &newVelocity);
    void setRotation(const Rotation &newRotation);

    Vector getVelocity();
    Rotation getRotation();
    float getRadius() const;

    RenderInformation getRenderInformation() override;

    bool getShouldBeDestroyed() const;
    void setShouldBeDestroyed(bool shouldBeDestroyed);

  private:
    Vector velocity{};
    Rotation rotation{};
    float radius = MIN_ROCK_SIZE;
    bool shouldBeDestroyed = false;
};

#endif // SURVIVING_SARNTAL_ROCK_H