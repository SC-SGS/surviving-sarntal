//
// Created by bjoern on 5/21/24.
//

#include "CollisionDetector.hpp"

CollisionDetector::CollisionDetector(World &world) : world(world) {}

void CollisionDetector::detectCollisions() const {}

bool CollisionDetector::rocksCollide(RockClass &r1, RockClass &r2) {
    const Vector pos1 = r1.getPosition();
    const float rad1 = r1.getRadius();
    const Vector pos2 = r1.getPosition();
    const float rad2 = r1.getRadius();
    return (pos1.distanceTo(pos2) <= rad1 + rad2);
}

bool CollisionDetector::isPlayerHitByRock(RockClass &rock) {
    const auto rockPosition = rock.getPosition();
    const auto rad = rock.getRadius();
    // hiker position is at his feet, but here we use the center of mass (center of the rectangle) as position
    const auto adjustedHikerPosition = this->world.getHiker().getPosition() + this->world.getHiker().getHeight() / 2;
    const auto hikerHeight = this->world.getHiker().getHeight();
    const auto hikerWidth = this->world.getHiker().getWidth();
    const auto xCenterDistanceAbs = std::abs(adjustedHikerPosition.x - rockPosition.x);
    const auto yCenterDistanceAbs = std::abs(adjustedHikerPosition.y - rockPosition.y);
    bool isHit = false;

    if (xCenterDistanceAbs > hikerWidth / 2 + rad || yCenterDistanceAbs > hikerHeight / 2 + rad) {

    } else if (xCenterDistanceAbs <= hikerWidth / 2 || yCenterDistanceAbs <= hikerHeight / 2) {
        isHit = true;
    } else {
        const auto cornerDistanceSq =
            std::pow(xCenterDistanceAbs - hikerWidth / 2, 2) + std::pow(yCenterDistanceAbs - hikerHeight / 2, 2);
        if (cornerDistanceSq <= std::pow(rad, 2)) {
            isHit = true;
        }
    }
    return isHit;
};

Vector CollisionDetector::getNormal(const std::size_t idx, const Vector rockPos) const {
    // determine closer vertex
    Position vertexOther = this->world.getMountain().getVertex((idx - 1) % MountainClass::NUMBER_OF_VERTICES);
    const Position vertexRight = this->world.getMountain().getVertex((idx + 1) % MountainClass::NUMBER_OF_VERTICES);
    if (rockPos.distanceTo(vertexOther) > rockPos.distanceTo(vertexRight)) {
        // else already correct
        vertexOther = vertexRight;
    }
    // calc distances
    const Position vertex = this->world.getMountain().getVertex(idx);
    const Vector slope = vertex - vertexOther;
    // compute normal from distances via rotation
    // R =  (  0   -1  )
    //      (  1    0  )
    Vector normal = {-slope.y, slope.x};
    if (normal.y < 0) {
        normal = normal * -1.f;
    }
    const float_type normalization = std::sqrt(normal * normal);
    return normal / normalization;
}