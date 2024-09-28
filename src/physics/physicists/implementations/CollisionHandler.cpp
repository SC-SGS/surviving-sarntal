//
// Created by bjoern on 5/21/24.
//

#include "../CollisionHandler.hpp"
#include "../../../output/graphics/Renderer.h"

CollisionHandler::CollisionHandler(World &world,
                                   const CollisionDetector &collisionDetector,
                                   AudioService &audioService,
                                   Renderer &renderer,
                                   const GameConstants &gameConstants)
    : world(world),
      collisionDetector(collisionDetector),
      audioService(audioService),
      renderer(renderer),
      gameConstants(gameConstants),
      deltaT(1),
      hapticsService(HapticsService::getInstance()) {}

void CollisionHandler::handleCollisions() const {
    this->playerCollisions();
    this->resolveRockCollisions();
}

void CollisionHandler::resolveRockCollisions() const {
    bool anyCollisions;
    size_t resolutionSteps = 0;
    do {
        anyCollisions = false;
        for (const std::shared_ptr<Rock> &rock : this->world.getRocks()) {
            anyCollisions = checkAndHandleTerrainCollision(*rock) || anyCollisions;
            for (const std::shared_ptr<Rock> &rock2 : this->world.getRocks()) {
                anyCollisions = checkAndHandleRockCollision(*rock, *rock2) || anyCollisions;
            }
        }
        resolutionSteps++;
    } while (anyCollisions && resolutionSteps < this->gameConstants.physicsConstants.maxNumberOfResolutionSteps);
    if (anyCollisions) {
        spdlog::debug("Not all collisions could be resolved in one physics step. Proceed with CAUTION!");
    }
}

bool CollisionHandler::checkAndHandleTerrainCollision(Rock &rock) const {
    bool collisionOccurred = false;
    const DynamicPolygonTerrainCollisionObject terrainCollision = this->collisionDetector.polygonTerrainCollision(rock);
    if (terrainCollision.isCollision) {
        resolveDynamicPolygonTerrainCollisionByDisplacement(terrainCollision,
                                                            this->gameConstants.physicsConstants.epsilon);
        collisionOccurred =
            applyImpulsesTerrainCollision(terrainCollision, this->gameConstants.physicsConstants.rockTerrainBounciness,
                                          this->gameConstants.physicsConstants.epsilon);
    }
    return collisionOccurred;
}

bool CollisionHandler::checkAndHandleRockCollision(Rock &rock, Rock &rock2) const {
    bool collisionOccurred = false;
    if (DynamicPolygonCollisionObject rockCollision;
        rock2 != rock && (rockCollision = this->collisionDetector.dynamicPolygonCollision(rock, rock2)).isCollision) {
        resolveDynamicPolygonCollisionByDisplacement(rockCollision, this->gameConstants.physicsConstants.epsilon);
        collisionOccurred =
            applyImpulsesPolygonCollision(rockCollision, this->gameConstants.physicsConstants.rockRockBounciness,
                                          this->gameConstants.physicsConstants.epsilon);
    }
    return collisionOccurred;
}

void CollisionHandler::resolveDynamicPolygonTerrainCollisionByDisplacement(
    const DynamicPolygonTerrainCollisionObject &terrainCollision, const floatType eps) {
    DynamicConvexPolygon &poly = *terrainCollision.poly;
    poly.setInterpolatedMovementState(terrainCollision.relativeTimeOfCollision);
    const floatType polyIsIncidentSignum = terrainCollision.isPolyIncident ? 1 : -1;
    const Vector newPosition =
        poly.getPosition() +
        terrainCollision.collisionDirection * (terrainCollision.collisionDepth * (1 + eps) * polyIsIncidentSignum);
    poly.setPosition(newPosition);
}

void CollisionHandler::resolveDynamicPolygonCollisionByDisplacement(const DynamicPolygonCollisionObject &collision,
                                                                    const floatType eps) {
    DynamicConvexPolygon &inc = *collision.polyAIncident;
    DynamicConvexPolygon &ref = *collision.polyBReference;
    inc.setInterpolatedMovementState(collision.relativeTimeOfCollision);
    ref.setInterpolatedMovementState(collision.relativeTimeOfCollision);
    const floatType sumOfMasses = inc.getMass() + ref.getMass();
    const Vector newPositionInc =
        inc.getPosition() +
        collision.collisionDirection * ((collision.collisionDepth / 2.0f) * (1 + eps) * (inc.getMass() / sumOfMasses));
    const Vector newPositionRef =
        ref.getPosition() -
        collision.collisionDirection * ((collision.collisionDepth / 2.0f) * (1 + eps) * (ref.getMass() / sumOfMasses));
    inc.setPosition(newPositionInc);
    ref.setPosition(newPositionRef);
}

bool CollisionHandler::applyImpulsesTerrainCollision(const DynamicPolygonTerrainCollisionObject &terrainCollision,
                                                     const floatType bounciness,
                                                     const floatType eps) {
    DynamicConvexPolygon &poly = *terrainCollision.poly;
    const ConvexPolygon &tri = terrainCollision.triangle.value();
    Vector contactPoint{};
    if (terrainCollision.isPolyIncident) {
        contactPoint = poly.getWorldSpaceVertices()[terrainCollision.collisionVertexIdx];
    } else {
        contactPoint = tri.getWorldSpaceVertices()[terrainCollision.collisionVertexIdx];
    }
    const floatType isPolyIncidentSignum = terrainCollision.isPolyIncident ? 1 : -1;
    const Vector polyVelocityAtContactPoint = poly.getVelocityAtPointInWorldSpace(contactPoint);
    const floatType relativeVelocityAtContactPoint =
        terrainCollision.collisionDirection.dot(polyVelocityAtContactPoint * isPolyIncidentSignum);
    if (relativeVelocityAtContactPoint > -eps)
        return false;
    const Vector normal = terrainCollision.collisionDirection;
    const floatType numerator = -(1 + bounciness) * relativeVelocityAtContactPoint;
    const floatType invMass = 1 / poly.getMass();
    const floatType invMomentOfInertia = 1 / poly.getMomentOfInertia();
    const Vector bodySpaceContactPoint = contactPoint - poly.getPosition();
    const floatType denominator =
        invMass +
        normal.dot(bodySpaceContactPoint.preCrossZScalar(bodySpaceContactPoint.cross(normal) * invMomentOfInertia));
    const floatType magnitudeOfImpulse = numerator / denominator;
    const Vector force = normal * magnitudeOfImpulse;
    poly.applyForce(force * isPolyIncidentSignum, bodySpaceContactPoint);
    return true;
}

bool CollisionHandler::applyImpulsesPolygonCollision(const DynamicPolygonCollisionObject &polygonCollision,
                                                     const floatType bounciness,
                                                     const floatType eps) {
    DynamicConvexPolygon &inc = *polygonCollision.polyAIncident;
    DynamicConvexPolygon &ref = *polygonCollision.polyBReference;
    const Vector contactPoint = inc.getWorldSpaceVertices()[polygonCollision.collisionVertexIdx];
    const Vector incVelocityAtContactPoint = inc.getVelocityAtPointInWorldSpace(contactPoint);
    const Vector refVelocityAtContactPoint = ref.getVelocityAtPointInWorldSpace(contactPoint);
    const Vector normal = polygonCollision.collisionDirection;
    const floatType relativeVelocityAtContactPoint = normal.dot(incVelocityAtContactPoint - refVelocityAtContactPoint);
    if (relativeVelocityAtContactPoint > -eps)
        return false;
    const floatType numerator = -(1 + bounciness) * relativeVelocityAtContactPoint;
    const floatType term1 = calculateDenominatorTermFor(inc, normal, contactPoint);
    const floatType term2 = calculateDenominatorTermFor(ref, normal, contactPoint);
    const floatType denominator = term1 + term2;
    const floatType magnitudeOfImpulse = numerator / denominator;
    const Vector force = normal * magnitudeOfImpulse;
    inc.applyForce(force, contactPoint - inc.getPosition());
    ref.applyForce(force * -1, contactPoint - ref.getPosition());
    return true;
}

floatType CollisionHandler::calculateDenominatorTermFor(const DynamicConvexPolygon &poly,
                                                        const Vector &normal,
                                                        const Vector &contactPoint) {
    const floatType invMass = 1 / poly.getMass();
    const floatType invMomentOfInertia = 1 / poly.getMomentOfInertia();
    const Vector bodySpaceContactPoint = contactPoint - poly.getPosition();
    const floatType scaryTerm =
        normal.dot(bodySpaceContactPoint.preCrossZScalar(bodySpaceContactPoint.cross(normal) * invMomentOfInertia));
    return invMass + scaryTerm;
}

void CollisionHandler::setDeltaT(const floatType deltaT) { this->deltaT = deltaT; }

// void CollisionHandler::playerCollisions() const {
//  NEW:
//  TODO parallelize and make efficient with linked cell or sth
//  for (auto &rock : this->world.getRocks()) {
//      CollisionObject collision =
//          CollisionDetector::polygonsCollide(rock, *this->world.getHiker().getCurrentBoundingBox());
//      if(collision.isCollision) {
//          // TODO player hit sound and rock explosion (texture, later actual explosion) should be somewhere else
//          this->audioService.playSound("rock-smash");
//          this->audioService.playSound("boom");
//          const int rockDmg = this->rockDamage(rock);
//          spdlog::debug("Player has hit with rock damage: {}", rockDmg);
//          HapticsService::rockRumble(rockDmg);
//          this->world.getHiker().doDamagePoints(rockDmg);
//          this->world.getHiker().setIsHit(true);
//          // TODO knockback issue
//          this->world.getHiker().addHitInformation(this->computeHitInformation(rock));
//          rock.setShouldBeDestroyed(true);
//      }
//  }
//  }

void CollisionHandler::playerCollisions() const {
    /* OLD:
    // TODO parallelize and make efficient with linked cell or sth
    for (auto &rock : this->world.getRocks()) {
        if (this->collisionDetector.isPlayerHitByRock(*rock)) {
            // TODO player hit sound and rock explosion (texture, later actual explosion) should be somewhere else
            this->audioService.playSound("rock-smash");
            this->audioService.playSound("boom");
            const int rockDmg = this->rockDamage(*rock);
            spdlog::debug("Player has hit with rock damage: {}", rockDmg);
            HapticsService::rockRumble(rockDmg);
            this->world.getHiker().doDamagePoints(rockDmg);
            this->world.getHiker().setIsHit(true);
            // TODO knockback issue
            this->world.getHiker().addHitInformation(this->computeHitInformation(rock));
            rock->setShouldBeDestroyed(true);
        }
    }*/
}

int CollisionHandler::rockDamage(const Rock &rock) const {
    const auto relativeRockSize =
        std::abs(49 * (rock.getBoundingBox().getWidth() / 2 - gameConstants.rockConstants.minRockSize));
    const auto rockSizeRange = (gameConstants.rockConstants.maxRockSize - gameConstants.rockConstants.minRockSize) + 1;
    const auto rockSizeFactor = static_cast<int>((relativeRockSize / rockSizeRange));
    const auto velocityFactor =
        static_cast<int>(1 + rock.getLinearMomentum().length() / gameConstants.rockConstants.velocityCap);
    return rockSizeFactor * velocityFactor;
}

/*void CollisionHandler::rockTerrainCollisions() {
    for (auto &rock : this->world.getRocks()) {
        // TODO don't create too many copies, but will be changed later anyways
        Rock virtualRock = this->getNextState(*rock);
        Line flightPath = {rock->getPosition(), virtualRock.getPosition()};
        std::vector<std::shared_ptr<Intersection>> intersections =
            this->world.getTerrain().getAllIntersections(flightPath);
        //  TODO Needs to be changed, I just want to test the mountain rendering
        if (!intersections.empty()) {
            Intersection &intersection = *intersections.front();
            this->rockTerrainCollision(*rock, intersection);
        }
        // if (this->world.getMountain().isInRange(virtualRock.getPosition().x)) {
        //    if (virtualRock.getPosition().y - this->world.getMountain().calculateYPos(virtualRock.getPosition().x) <
        //        virtualRock.getBoundingBox().width / 2) {
        //        this->rockTerrainCollision(rock);
        //    }
        //}
    }
}

void CollisionHandler::rockTerrainCollision(Rock &rock, Intersection &intersection) const {
    auto vel = rock.getLinearMomentum(); // TODO play terrain collision sound
    auto pos = rock.getPosition();
    const auto rad = rock.getBoundingBox().width / 2;
    floatType angularVelocity = rock.getAngularMomentum();
    floatType angularOffset = rock.getRotationAngle();
    // const auto normal = this->collisionDetector.getNormal(closestVertex.index, pos);
    const auto normal = intersection.normalAtIntersection;
    vel = vel.reflectOnNormal(normal, gameConstants.physicsConstants.rockTerrainDamping);
    const auto mass = std::pow(rad, 2);
    const Vector parallelVector = {-normal.y, normal.x};
    const auto velocityParallel = vel * parallelVector;
    angularVelocity +=
        static_cast<floatType>((gameConstants.rockConstants.gamma) * (velocityParallel - angularVelocity * rad) / mass);
    const auto maxAngularVelocity = gameConstants.rockConstants.maxAngularVelocity;
    if (angularVelocity >= maxAngularVelocity) {
        angularVelocity = maxAngularVelocity;
    }
    if (angularVelocity <= -maxAngularVelocity) {
        angularVelocity = -maxAngularVelocity;
    }
    angularOffset += this->deltaT * angularVelocity;
    rock.setLinearMomentum(vel); //  TODO maybe later explode rock here
    rock.setAngularMomentum(angularVelocity);
    rock.setRotationAngleRad(angularOffset);
    auto shake = (rock.getLinearMomentum().length() * rock.getBoundingBox().width / 2 *
                  this->gameConstants.visualConstants.rumbleIntensity) /
                 gameConstants.rockConstants.velocityCap;
    this->renderer.setShake(shake);
}

void CollisionHandler::rockRockCollisions() {
    for (auto &rock1 : this->world.getRocks()) {
        for (auto &rock2 : this->world.getRocks()) {
            // TODO remove unnecessary copies as far as possible
            Rock vRock1 = this->getNextState(*rock1);
            Rock vRock2 = this->getNextState(*rock2);
            if (CollisionDetector::rocksCollide(vRock1, vRock2)) {
                // TODO play rock collision sounds
                rockRockCollision(*rock1, *rock2);

                // TODO maybe explode rocks
            }
        }
    }
}
// NOLINTBEGIN
void CollisionHandler::rockRockCollision(Rock &rock1, Rock &rock2) {
    const floatType mass1 = powf(rock1.getBoundingBox().width / 2, 2);
    const floatType mass2 = powf(rock1.getBoundingBox().width / 2, 2);
    Vector vel1 = rock1.getLinearMomentum();
    Vector vel2 = rock2.getLinearMomentum();
    const Vector pos1 = rock1.getPosition();
    const Vector pos2 = rock2.getPosition();
    const Vector velDiff = vel1 - vel2;
    const Vector posDiff = pos1 - pos2;
    floatType angularVelocity1 = rock1.getAngularMomentum();
    floatType angularOffset1 = rock1.getRotationAngle();
    floatType angularVelocity2 = rock2.getAngularMomentum();
    floatType angularOffset2 = rock2.getRotationAngle();
    const floatType distanceSq = posDiff * posDiff;
    const floatType totalMass = mass1 + mass2;
    vel1 -=
        posDiff * 2 * mass2 * (velDiff * posDiff) / (distanceSq * totalMass + gameConstants.physicsConstants.epsilon);
    vel2 +=
        posDiff * 2 * mass1 * (velDiff * posDiff) / (distanceSq * totalMass + gameConstants.physicsConstants.epsilon);
    const Vector normal = {pos2.y - pos1.y, pos1.x - pos2.x};
    const floatType relativeSurfaceVelocity =
        angularVelocity2 * rock2.getBoundingBox().width / 2 - angularVelocity1 * rock1.getBoundingBox().width / 2;
    angularVelocity1 +=
        gameConstants.rockConstants.gamma * std::abs((normal * vel1)) * relativeSurfaceVelocity / (2 * mass1);
    angularVelocity2 -=
        gameConstants.rockConstants.gamma * std::abs((normal * vel2)) * relativeSurfaceVelocity / (2 * mass2);
    angularVelocity1 = capAngularVelocity(angularVelocity1);
    angularVelocity2 = capAngularVelocity(angularVelocity2);
    rock1.setLinearMomentum(vel1);
    rock2.setLinearMomentum(vel2);
    rock1.setAngularMomentum(angularVelocity1);
    rock2.setAngularMomentum(angularVelocity2);
    rock1.setRotationAngleRad(angularOffset1);
    rock2.setRotationAngleRad(angularOffset2);
};
// NOLINTEND
Rock CollisionHandler::getNextState(Rock &rock) const {
    const auto pos = rock.getPosition();
    const auto newPos = pos + rock.getLinearMomentum() * this->deltaT;
    const floatType newAngularOffset = rock.getRotationAngle() + rock.getAngularMomentum() * this->deltaT;
    // TODO we need not copy the BodySpaceVertices and Texture Coordinates each time, a dynamic attribute object would
    // be better
    return {newPos,
            rock.getBodySpaceVertices(),
            rock.getTextureCoordinates(),
            rock.getMass(),
            rock.getDensity(),
            rock.getMomentOfInertia(),
            rock.getDynamicProperties()};
}

floatType CollisionHandler::capAngularVelocity(const floatType angVel) const {
    const auto maxAngularVelocity = gameConstants.rockConstants.maxAngularVelocity;
    if (angVel >= maxAngularVelocity) {
        return maxAngularVelocity;
    }
    if (angVel <= -maxAngularVelocity) {
        return -maxAngularVelocity;
    }
    return angVel;
}*/

HitInformation CollisionHandler::computeHitInformation(std::shared_ptr<Rock> &rock) const {
    Vector linearMomentum = rock->getLinearMomentum();
    Vector knockback = linearMomentum / this->gameConstants.hikerConstants.mass;
    int steps = static_cast<int>(knockback.length());
    return {knockback, steps, 0};
}
