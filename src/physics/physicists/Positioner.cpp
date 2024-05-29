//
// Created by bjoern on 5/21/24.
//

#include "Positioner.hpp"

Positioner::Positioner(World &world, const float deltaT) : world(world), deltaT(deltaT) {}

void Positioner::updatePositions() const {
    this->updateMonsterPosition();
    this->updateWorldBorderPosition();
    this->updateRockPositions();
    this->updateHikerPosition();
};

void Positioner::updateMonsterPosition() const {
    auto xPos = this->world.getMonster().getXPosition();
    this->world.getMonster().setXPosition(xPos + KILL_BAR_VELOCITY * this->deltaT);
}

void Positioner::updateWorldBorderPosition() const {
    auto xMin = this->world.getMinX();
    auto xMax = this->world.getMaxX();
    auto delta = KILL_BAR_VELOCITY * this->deltaT;
    this->world.setMinX(xMin + delta);
    this->world.setMaxX(xMax + delta);
}

void Positioner::updateRockPositions() const {
    for (RockClass *rock : this->world.getRocks()) {
        auto pos = rock->getPosition();
        pos += rock->getVelocity() * this->deltaT;
        rock->setPosition(pos);
        auto rot = rock->getRotation();
        rot.angular_offset += rot.angular_velocity * this->deltaT;
        rock->setRotation(rot);
    }
}

void Positioner::updateHikerPosition() const {
    // TODO maybe we want to switch back to float type, but clang won't allow it as of now
    float knockback = 0;
    Hiker &hiker = this->world.getHiker();
    // TODO this should go into the collision handler, but there might be problems with the event processor so let's
    // leave it here until we take care of the physics
    if (hiker.getIsHit()) {
        const float radius = hiker.getHitInformation().radiusRock;
        const float rockVelocity = hiker.getHitInformation().velocityRock;
        // TODO KNOCKBACK could vary with rock type, e.g. small rocks with little damage but massive knockback etc
        // TODO also, why is knockback only in x direction?
        knockback = rockVelocity * KNOCKBACKCONST * radius;
        // TODO I think we should remove all unnecessary prints
        std::cout << "hit: " << knockback << std::endl;
        int counter = hiker.getHitInformation().countingVariable;
        counter++;
        // TODO I don't like the HitInformation
        hiker.setHitInformation({radius, rockVelocity, counter});
        // TODO this 50 feels like a poor implementation choice and should at least be some sort of constant
        // TODO like KNOCKBACK_TIME
        // TODO or, we should do it via forces and impulses so we don't need that at all, i.e. the player input is a
        // TODO force not a set velocity
        if (counter >= 50) {
            hiker.setIsHit(false);
            hiker.setHitInformation({0.0f, 0.0f, 0});
        }
    }

    auto pos = hiker.getPosition();
    const auto vel = hiker.getVelocity();
    if (hiker.getHikerMovement().getState() == HikerMovement::MovementState::IN_AIR) {
        pos.x += knockback + AIR_MOVEMENT_SPEED_FACTOR * vel.x * this->deltaT;
        const auto terrainY = this->world.getMountain().getYPosFromX(pos.x);
        const auto airY = pos.y + vel.y * this->deltaT;
        if (airY > terrainY) {
            pos.y = airY;
        } else {
            pos.y = terrainY;
            hiker.getHikerMovement().setState(HikerMovement::MovementState::MOVING);
            hiker.getHikerMovement().setCanJumpAgain(true);
            hiker.getHikerMovement().setLastJump(0.0);
            // TODO I think graphics does that already
            /* it.entity(0).remove<graphics::BillboardComponent>();
            it.entity(0).set([&](graphics::AnimatedBillboardComponent &c) {
                c = {0};
                c.billUp = {0.0f, 0.0f, 1.0f};
                c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
                c.resourceHandle =
                    it.world().get_mut<graphics::Resources>()->textures.load("../assets/texture/player_walk.png");
                c.width = HIKER_WIDTH; // TODO?
                c.height = HIKER_HEIGHT;
                c.current_frame = 0;
                c.numFrames = 4;
            });
            */
        }
        // TODO this whole speedfactor shebang needs thorough examination and fiddling in later stages
    } else if (hiker.getHikerMovement().getDirection() != HikerMovement::Direction::NEUTRAL) {
        const auto nextXPos = vel.x * this->deltaT + pos.x;
        const auto nextYPos = this->world.getMountain().getYPosFromX(nextXPos);
        Vector direction = {nextXPos - pos.x, nextYPos - pos.y};
        const float length = direction.length();
        const float slope = direction.y / direction.x;
        const float speedFactor = getSpeedFactor(slope);
        // TODO this speed formula is sus
        pos.x += (this->deltaT * std::abs(vel.x * speedFactor) / length) * direction.x + knockback;
        pos.y = this->world.getMountain().getYPosFromX(pos.x);
    } else {
        pos.x += knockback;
        pos.y = this->world.getMountain().getYPosFromX(pos.x);
    }
    if (pos.x > this->world.getMonster().getXPosition() + PLAYER_RIGHT_BARRIER_OFFSET) {
        pos.x = this->world.getMonster().getXPosition() + PLAYER_RIGHT_BARRIER_OFFSET;
    }
    hiker.setPosition(pos);
}

float Positioner::getSpeedFactor(const float slope) {
    if (slope <= SLOWEST_NEG_SLOPE) {
        return MIN_SPEED_NEG_SLOPE;
    }
    if (slope <= FASTEST_NEG_SCOPE) {
        return MountainClass::linearInterpolation(slope, {SLOWEST_NEG_SLOPE, MIN_SPEED_NEG_SLOPE},
                                                  {FASTEST_NEG_SCOPE, MAX_SPEED_NEG_SLOPE});
    }
    if (slope <= 0) {
        return MountainClass::linearInterpolation(slope, {FASTEST_NEG_SCOPE, MAX_SPEED_NEG_SLOPE}, {0, 1});
    }
    if (slope <= SLOWEST_POS_SCOPE) {
        return MountainClass::linearInterpolation(slope, {0, 1}, {SLOWEST_POS_SCOPE, MIN_SPEED_POS_SCOPE});
    }

    return MIN_SPEED_POS_SCOPE;
};