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
    this->resolveRockCollisions();
    this->playerCollisions();
}

void CollisionHandler::resolveRockCollisions() const {
    bool anyCollisions;
    size_t resolutionSteps = 0;
    do {
        anyCollisions = false;
        const auto &rocks = this->world.getRocks();
        for (auto iter1 = rocks.begin(); iter1 != rocks.end(); ++iter1) {
            const std::shared_ptr<Rock> &rock = *iter1;
            anyCollisions = checkAndHandleTerrainCollision(*rock) || anyCollisions;
            this->checkAndHandlePlayerRockCollision(*rock);
            auto iter2 = iter1;
            ++iter2;
            for (; iter2 != rocks.end(); ++iter2) {
                const std::shared_ptr<Rock> &otherRock = *iter2;
                anyCollisions = checkAndHandleRockCollision(*rock, *otherRock) || anyCollisions;
            }
        }
        resolutionSteps++;
    } while (anyCollisions && resolutionSteps < this->gameConstants.physicsConstants.maxNumberOfResolutionSteps);
    if (anyCollisions) {
        spdlog::debug("Not all collisions could be resolved in one physics step. Proceed with CAUTION!");
    }
}

bool CollisionHandler::checkAndHandleTerrainCollision(Rock &rock) const {
    if (rock.getLinearMomentum().length() / rock.getMass() < this->gameConstants.physicsConstants.epsilon)
        return false;
    bool collisionOccurred = false;
    const DynamicPolygonTerrainCollisionObject terrainCollision = this->collisionDetector.polygonTerrainCollision(rock);
    if (terrainCollision.isCollision) {
        resolveDynamicPolygonTerrainCollisionByDisplacement(terrainCollision,
                                                            this->gameConstants.physicsConstants.epsilon);
        collisionOccurred =
            applyImpulsesTerrainCollision(terrainCollision, this->gameConstants.physicsConstants.rockTerrainBounciness,
                                          this->gameConstants.physicsConstants.epsilon);
        // TODO rework shake and threshold (and maybe sound) dependent on rock type
        if (collisionOccurred &&
            rock.getLinearMomentum().length() > this->gameConstants.audioConstants.effectsThreshold) {
            this->audioService.playSound("rock-terrain_smash");
            const auto shake =
                (graphics::UNIT_TO_PIXEL_RATIO * rock.getLinearMomentum().length() * rock.getBoundingBox().getWidth() /
                 2 * this->gameConstants.visualConstants.rumbleIntensity) /
                gameConstants.rockConstants.maxSpawnLinearMomentum;
            this->renderer.setShake(shake);
        }
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
        if (collisionOccurred && rock.getLinearMomentum().length() + rock2.getLinearMomentum().length() >
                                     2 * this->gameConstants.audioConstants.effectsThreshold) {
            this->audioService.playSound("rock-smash");
        }
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
    if (terrainCollision.isPolyIncident)
        contactPoint = poly.getWorldSpaceVertices()[terrainCollision.collisionVertexIdx];
    else
        contactPoint = tri.getWorldSpaceVertices()[terrainCollision.collisionVertexIdx];
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
    const Vector normal = polygonCollision.collisionDirection;
    const Vector relativeVelocityAtContactPoint =
        inc.getVelocityAtPointInWorldSpace(contactPoint) - ref.getVelocityAtPointInWorldSpace(contactPoint);
    const floatType normalMagnitudeOfRelativeVelocityAtContactPoint = normal.dot(relativeVelocityAtContactPoint);
    if (normalMagnitudeOfRelativeVelocityAtContactPoint > -eps)
        return false;
    const floatType numerator = -(1 + bounciness) * normalMagnitudeOfRelativeVelocityAtContactPoint;
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

void CollisionHandler::playerCollisions() const {
    auto &hiker = this->world.getHiker();
    if (!hiker.getIsAlive())
        return;
    if (hiker.getHikerMovement().getState() != HikerMovement::IN_AIR && hiker.getVelocity().x == 0) {
        if (hiker.getShouldUncrouch() && this->hikerCanUncrouch()) {
            hiker.uncrouch();
        }
        return;
    }
    bool anyCollisions = false;
    size_t numberOfIterations = 0;
    do {
        anyCollisions =
            this->resolveHikerTerrainCollision(hiker.getHikerMovement().getState() == HikerMovement::IN_AIR);
        numberOfIterations++;
    } while (anyCollisions && numberOfIterations < this->gameConstants.physicsConstants.maxNumberOfResolutionSteps);
    if (anyCollisions) {
        spdlog::info("Couldn't resolve collisions.");
    }
    if (hiker.getShouldUncrouch() && this->hikerCanUncrouch()) {
        hiker.uncrouch();
    }
    this->enforceWorldLimitOnHiker();
}

bool CollisionHandler::resolveHikerTerrainCollision(const bool inAir) const {
    bool collisionOccurred = false;
    Hiker &hiker = this->world.getHiker();
    DynamicConvexPolygon &hitbox = *hiker.getCurrentHitbox();
    const auto terrainColl = this->collisionDetector.polygonTerrainCollision(hitbox);
    if (terrainColl.isCollision) {
        if (inAir) {
            resolveDynamicPolygonTerrainCollisionByDisplacement(terrainColl,
                                                                this->gameConstants.physicsConstants.epsilon);
            collisionOccurred =
                applyImpulsesTerrainCollision(terrainColl, this->gameConstants.physicsConstants.rockTerrainBounciness,
                                              this->gameConstants.physicsConstants.epsilon);
            hiker.setPosition(terrainColl.poly->getPosition() - hiker.getCurrentHitboxDelta());
            hiker.move({0.f, 0.f});
            hiker.setVelocity(terrainColl.poly->getLinearMomentum() / this->gameConstants.hikerConstants.mass);
            hiker.resetHitboxAngularMomentum();
        } else {
            collisionOccurred = false;
            hiker.setPosition(terrainColl.poly->getDynamicProperties().lastPosition - hiker.getCurrentHitboxDelta());
            hiker.setVelocity({0.f, 0.f});
            spdlog::debug("Hiker hit his head");
        }
        hiker.setKnockback({0.f});
    }
    this->clampHikerIfNeeded();
    return collisionOccurred;
}

void CollisionHandler::enforceWorldLimitOnHiker() const {
    Hiker &hiker = this->world.getHiker();
    Vector pos = hiker.getPosition();
    const float rightBorder = this->world.getMinX() + (this->world.getMaxX() - this->world.getMinX()) * 0.9f;
    if (pos.x > rightBorder) {
        pos.x = rightBorder;
        if (hiker.getHikerMovement().getState() != HikerMovement::MovementState::IN_AIR) {
            pos.y = this->world.getTerrain().mapHeightToTerrain(pos) + NUMERIC_EPSILON;
        }
    }
    hiker.setPosition(pos);
}

int CollisionHandler::rockDamage(const Rock &rock) const {
    if (rock.getLinearMomentum().length() / rock.getMass() < this->gameConstants.physicsConstants.epsilon)
        return 0.f;
    const auto relativeRockSize =
        std::abs(49 * (rock.getBoundingBox().getWidth() / 2 - gameConstants.rockConstants.minRockSize));
    const auto rockSizeRange = (gameConstants.rockConstants.maxRockSize - gameConstants.rockConstants.minRockSize) + 1;
    const auto rockSizeFactor = static_cast<int>((relativeRockSize / rockSizeRange));
    const auto momentumFactor =
        static_cast<int>(1 + rock.getLinearMomentum().length() / (gameConstants.rockConstants.maxSpawnLinearMomentum));
    return rockSizeFactor * momentumFactor;
}

Vector CollisionHandler::computeKnockback(const DynamicPolygonCollisionObject &coll) const {
    const Hiker &hiker = this->world.getHiker();
    Vector linearMomentum{};
    floatType isHikerIncidentSignum;
    if (*hiker.getCurrentHitbox() == *coll.polyAIncident) {
        linearMomentum =
            coll.polyBReference->getLinearMomentum() * this->gameConstants.hikerConstants.knockbackIntensity;
        isHikerIncidentSignum = 1;
    } else {
        linearMomentum =
            coll.polyAIncident->getLinearMomentum() * this->gameConstants.hikerConstants.knockbackIntensity;
        isHikerIncidentSignum = -1;
    }
    const Vector knockback = coll.collisionDirection * linearMomentum.length() *
                             isHikerIncidentSignum; // / this->gameConstants.hikerConstants.mass;
    return knockback;
}

void CollisionHandler::checkAndHandlePlayerRockCollision(Rock &rock) const {
    auto &hiker = this->world.getHiker();
    if (!hiker.getIsAlive())
        return;
    auto const &collision = this->collisionDetector.playerRockCollision(rock);
    if (collision.isCollision) {
        this->audioService.playSound("rock-smash");
        this->audioService.playSound("boom");
        const int rockDmg = this->rockDamage(rock);
        spdlog::debug("Player has hit with rock damage: {}", rockDmg);
        hiker.doDamagePoints(rockDmg);
        hiker.setKnockback(this->world.getHiker().getKnockback() + this->computeKnockback(collision));
        rock.setShouldBeDestroyed(true);
        if (hiker.getIsAlive()) {
            HapticsService::rockRumble(rockDmg);
        }
    }
}

bool CollisionHandler::hikerCanUncrouch() const {
    return !this->collisionDetector.polygonTerrainCollision(*this->world.getHiker().getHitboxWalking()).isCollision;
}

void CollisionHandler::clampHikerIfNeeded() const {
    Hiker &hiker = this->world.getHiker();
    const floatType terrainHeight = this->world.getTerrain().mapToClosestTopTerrain(hiker.getPosition());
    if (hiker.getPosition().y < terrainHeight + this->gameConstants.physicsConstants.epsilon) {
        hiker.setPosition(Vector{hiker.getPosition().x, terrainHeight + this->gameConstants.physicsConstants.epsilon});
        hiker.setMoving();
    }
}
