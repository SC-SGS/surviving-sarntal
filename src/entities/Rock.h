//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCK_H
#define SURVIVING_SARNTAL_ROCK_H

#include "../utilities/vector.h"
#include "DynamicPolygon.h"

#include "RenderedEntity.h"

/**
 * This class represents a rock in the game. A rock is simulated by a certain point (position) with a radius
 * and the attributes velocity and rotation.
 */
class Rock : public DynamicPolygon {
  public:
    Rock(const Vector &position, const std::vector<Vector> &vertices, const std::vector<Vector2> &textureCoordinates,
         floatType mass, floatType momentOfInertia, const DynamicProperties &dynamicProperties);
    ~Rock() override = default;

    RenderInformation getRenderInformation() const override;

    bool getShouldBeDestroyed() const;
    void setShouldBeDestroyed(bool shouldBeDestroyedNew);

  private:
    bool shouldBeDestroyed = false;
};

#endif // SURVIVING_SARNTAL_ROCK_H