//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCK_H
#define SURVIVING_SARNTAL_ROCK_H

#include "../components/vector.h"
#include "../graphics/render_information/RenderInformation.h"
#include "RenderedEntity.h"

class RockClass : public RenderedEntity {
  public:
    explicit RockClass(Vector position) : RenderedEntity(position) { this->position = position; }
    RockClass(Vector position, Vector velocity, Rotation rotation, float radius) : RenderedEntity(position) {
        this->position = position;
        this->velocity = velocity;
        this->rotation = rotation;
        this->radius = radius;
    };
    void setVelocity(Vector &newVelocity);
    void setRotation(Rotation &newRotation);

    Vector getVelocity();
    Rotation getRotation();
    float getRadius() const;

    RenderInformation getRenderInformation() override;

  private:
    Vector velocity{};
    Rotation rotation{};
    float radius = MIN_ROCK_SIZE;
};

#endif // SURVIVING_SARNTAL_ROCK_H