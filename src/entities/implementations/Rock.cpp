//
// Created by Aleksis Vezenkov on 02.05.24.
//

#include "../Rock.h"

#include "../../utilities/vector.h"
#include <cmath>

Rock::Rock(const Vector &position,
           const std::vector<Vector> &vertices,
           const std::vector<Vector2> &textureCoordinates,
           const floatType mass,
           const floatType density,
           const floatType momentOfInertia,
           const DynamicProperties &dynamicProperties)
    : DynamicConvexPolygon(position, vertices, textureCoordinates, mass, density, momentOfInertia, dynamicProperties),
      type(NORMAL_ROCK) {}

Rock::Rock(const Vector &position,
           const DynamicConvexPolygon &polygon,
           const DynamicProperties &properties,
           const RockType &rockType)
    : DynamicConvexPolygon(position,
                           polygon.getBodySpaceVertices(),
                           polygon.getTextureCoordinates(),
                           polygon.getMass(),
                           polygon.getDensity(),
                           polygon.getMomentOfInertia(),
                           properties),
      type(rockType) {}

RenderInformation Rock::getRenderInformation() const {
    if (animation.frames == 0) {
        return RenderInformation{Vector2(this->position),
                                 {0, 0},
                                 0.f,
                                 0.f,
                                 this->dynamicProperties.rotationAngleRad * static_cast<floatType>(180.0f / M_PI),
                                 this->getTextureName()};
    }
    const AABB &aabb = this->getBoundingBox();
    return RenderInformation{Vector2(this->position),
                             {0, 0},
                             aabb.getTopRight().x - aabb.getBottomLeft().x,
                             aabb.getTopRight().y - aabb.getBottomLeft().y,
                             this->dynamicProperties.rotationAngleRad,
                             "explosion",
                             {25, 0, 0.1, 0}};
}

std::string Rock::getTextureName() const {
    switch (this->type) {
    case NORMAL_ROCK:
        return "rockRectangle";
    case SNOW_ROCK:
        return "snowRock";
    case ICE_ROCK:
        return "iceRock";
    case HEAVY_ROCK:
        return "heavyRock";
    case LAVA_ROCK:
        return "lavaRock";
    case CRYSTAL_ROCK:
        return "crystalRock";
    default:
        return "";
    }
}
bool Rock::getShouldBeDestroyed() const { return this->shouldBeDestroyed; }
void Rock::setShouldBeDestroyed(const bool shouldBeDestroyedNew) { this->shouldBeDestroyed = shouldBeDestroyedNew; }
