//
// Created by Aleksis Vezenkov on 02.05.24.
//

#ifndef SURVIVING_SARNTAL_ROCK_H
#define SURVIVING_SARNTAL_ROCK_H

#include "../geometry/DynamicConvexPolygon.h"
#include "../utilities/vector.h"

#include "RenderedEntity.h"

/**
 * Rock types that occur during the game.
 */
enum RockType { NORMAL_ROCK = 0, SNOW_ROCK = 1, ICE_ROCK = 2, HEAVY_ROCK = 3, LAVA_ROCK = 4, CRYSTAL_ROCK = 5 };

/**
 * This class represents a rock in the game. A rock is simulated by a certain point (position) with a radius
 * and the attributes velocity and rotation.
 */
class Rock : public DynamicConvexPolygon {
  public:
    Rock(const Vector &position,
         const std::vector<Vector> &vertices,
         const std::vector<Vector2> &textureCoordinates,
         floatType mass,
         floatType density,
         floatType momentOfInertia,
         const DynamicProperties &dynamicProperties);
    Rock(const Vector &position,
         const DynamicConvexPolygon &polygon,
         const DynamicProperties &properties,
         const RockType &rockType);
    ~Rock() override = default;

    RenderInformation getRenderInformation() const override;

    bool getShouldBeDestroyed() const;
    void setShouldBeDestroyed(bool shouldBeDestroyedNew);

  private:
    bool shouldBeDestroyed = false;
    RockType type;

    std::string getTextureName() const;
};

#endif // SURVIVING_SARNTAL_ROCK_H