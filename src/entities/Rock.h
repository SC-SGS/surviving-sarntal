//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCK_H
#define SURVIVING_SARNTAL_ROCK_H

#include "../components/vector.h"
#include "RenderInformation.h"
#include "RenderedEntity.h"

class RockClass : public RenderedEntity {
  public:
    void setVelocity(Vector &newVelocity);
    void setRotation(Rotation &newRotation);

    Vector getVelocity();
    Rotation getRotation();
    float getRadius();

  private:
    Vector velocity;
    Rotation rotation;
    float radius;
};

#endif // SURVIVING_SARNTAL_ROCK_H