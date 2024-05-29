//
// Created by bjoern on 5/21/24.
//

#include "EventProcessor.hpp"

// TODO why is the game event list passed to all functions and never used???

EventProcessor::EventProcessor(World &world, const InputHandler &inputHandler)
    : world(world), inputHandler(inputHandler) {
    functionMappings = {
        {{AXIS_MODIFICATION, ITEM_SWITCH, 0}, &EventProcessor::switchItem},
        {{AXIS_MODIFICATION, MOVEMENT_X, 0}, &EventProcessor::moveX},
        {{AXIS_MODIFICATION, MOVEMENT_Y, 0}, &EventProcessor::moveY},
        {{JUMP, NO_AXIS, 0}, &EventProcessor::jump},
        {{CROUCH, NO_AXIS, 0}, &EventProcessor::crouch},
        {{UNCROUCH, NO_AXIS, 0}, &EventProcessor::uncrouch},
        {{ITEM_PICK, NO_AXIS, 0}, &EventProcessor::pickItem},
        {{ITEM_USE, NO_AXIS, 0}, &EventProcessor::useItem},
        {{ITEM_DROP, NO_AXIS, 0}, &EventProcessor::dropItem},
        {{PAUSE, NO_AXIS, 0}, &EventProcessor::pause},
        {{FULLSCREEN, NO_AXIS, 0}, &EventProcessor::fullscreen},
        {{SPECIAL_ABILITY, NO_AXIS, 0}, &EventProcessor::specialAbility},
        {{TOGGLE_DEBUG, NO_AXIS, 0}, &EventProcessor::toggleDebug},
        {{NO_EVENT, NO_AXIS, 0}, &EventProcessor::noEvent},
    };
}

void EventProcessor::processEvents() {
    const auto events = this->inputHandler.getEvents();
    for (auto event : events) {
        const EventFunction func = functionMappings.at(event);
        (this->*func)(event);
    }
}

// TODO: Let output handle sound, graphics?
void EventProcessor::crouch(const GameEvent event) const {
    auto hiker = this->world.getHiker();
    auto hikerMovement = this->world.getHiker().getHikerMovement();
    if (hikerMovement.getState() == HikerMovement::MOVING) {
        hikerMovement.setState(HikerMovement::CROUCHED);
        hiker.setHeight(DUCKED_HIKER_HEIGHT);
        hiker.setWidth(DUCKED_HIKER_WIDTH);
        // TODO remove
        /*RenderInformation info = {{0.0f, 0.0f, 1.0f},
                                  {0.0f, 0.0f, -HIKER_HEIGHT / 2},
                                  static_cast<int>(DUCKED_HIKER_WIDTH),
                                  static_cast<int>(DUCKED_HIKER_HEIGHT),
                                  LoadTexture("../../assets/texture/hiker_duck.png")};
        hiker.setRenderInformation(info);*/
        // it.entity(0).get_mut<graphics::RectangleShapeRenderComponent>()->height = DUCKED_HIKER_HEIGHT;
        // it.entity(0).get_mut<graphics::RectangleShapeRenderComponent>()->width = DUCKED_HIKER_WIDTH;
        // it.entity(0).remove<graphics::AnimatedBillboardComponent>();
        // it.entity(0).set([&](graphics::BillboardComponent &c) {
        //     c = {0};
        //     c.billUp = {0.0f, 0.0f, 1.0f};
        //     c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
        //     c.resourceHandle =
        //         it.world().get_mut<graphics::Resources>()->textures.load("../assets/texture/hiker_duck.png");
        //     c.width = DUCKED_HIKER_WIDTH; // TODO?
        //     c.height = DUCKED_HIKER_HEIGHT;
        // });
    }
}

// TODO: Let output handle sound, graphics?
void EventProcessor::uncrouch(const GameEvent event) const {
    auto hiker = this->world.getHiker();
    auto hikerMovement = this->world.getHiker().getHikerMovement();
    if (hikerMovement.getState() == HikerMovement::CROUCHED) {
        hikerMovement.setState(HikerMovement::MOVING);
        hiker.setHeight(HIKER_HEIGHT);
        hiker.setWidth(HIKER_WIDTH);
        // TODO remove
        /*RenderInformation info = {{0.0f, 0.0f, 1.0f},
                                  {0.0f, 0.0f, -HIKER_HEIGHT / 2},
                                  static_cast<int>(HIKER_WIDTH),
                                  static_cast<int>(HIKER_HEIGHT),
                                  LoadTexture("../../assets/texture/hiker_walk.png")};
        hiker.setRenderInformation(info);*/
        // it.entity(0).get_mut<graphics::RectangleShapeRenderComponent>()->height = HIKER_HEIGHT;
        // it.entity(0).get_mut<graphics::RectangleShapeRenderComponent>()->width = HIKER_WIDTH;
        // it.entity(0).remove<graphics::BillboardComponent>();
        // it.entity(0).set([&](graphics::AnimatedBillboardComponent &c) {
        //     c = {0};
        //     c.billUp = {0.0f, 0.0f, 1.0f};
        //     c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
        //     c.resourceHandle =
        //         it.world().get_mut<graphics::Resources>()->textures.load("../assets/texture/player_walk.png");
        //     c.width = HIKER_WIDTH; // TODO?
        //     c.height = HIKER_HEIGHT;
        //     c.current_frame = 0;
        //     c.numFrames = 4;
        // });
    }
}

// TODO: use method from InventoryHandler to check for collectible items
void EventProcessor::pickItem(const GameEvent event) const {
    const auto items = this->world.getNearbyItems();
    for (auto const &item : items) {
        if (!Item::getItemInformation(item.getItemType()).autoCollect) {
            this->world.getInventory().pickup(item.getItemType());
        }
    }
}

void EventProcessor::dropItem(GameEvent event) const { this->world.getInventory().drop(); }

void EventProcessor::useItem(const GameEvent event) const {
    this->world.useItem(this->world.getInventory().getSelectedItem());
}
// TODO I removed this getHiker().useSelectedItem(this->world.getHiker()); }

// TODO: Let output handle sound, graphics?, change to use ressource manager
void EventProcessor::jump(GameEvent event) const {
    auto hiker = this->world.getHiker();
    auto hikerMovement = hiker.getHikerMovement();
    if (hikerMovement.getState() == HikerMovement::CROUCHED) {
        return;
    }
    if (hikerMovement.getState() != HikerMovement::IN_AIR) {
        hikerMovement.setLastJump(0.0);
    }
    if (hikerMovement.getLastJump() < 1.5 && hikerMovement.getCanJumpAgain()) {
        // PlaySound(jump_sound);
        auto vel = hiker.getVelocity();
        vel.y = JUMP_VELOCITY_CONSTANT;
        hiker.setVelocity(vel);
        if (hikerMovement.getState() == HikerMovement::IN_AIR) {
            hikerMovement.setCanJumpAgain(false);
        }
        hikerMovement.setState(HikerMovement::IN_AIR);
        // TODO remove
        /*RenderInformation info = {{0.0f, 0.0f, 1.0f},
                                  {0.0f, 0.0f, -HIKER_HEIGHT / 2},
                                  static_cast<int>(HIKER_WIDTH),
                                  static_cast<int>(HIKER_HEIGHT),
                                  LoadTexture("../../assets/texture/hiker_jump.png")};
        hiker.setRenderInformation(info);*/
        // it.entity(0).remove<graphics::AnimatedBillboardComponent>();
        // it.entity(0).set([&](graphics::BillboardComponent &c) {
        //     c = {0};
        //     c.billUp = {0.0f, 0.0f, 1.0f};
        //     c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
        //     c.resourceHandle =
        //         it.world().get_mut<graphics::Resources>()->textures.load("../assets/texture/hiker_jump.png");
        //     c.width = HIKER_WIDTH; // TODO?
        //     c.height = HIKER_HEIGHT;
        // });
    }
}

void EventProcessor::pause(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::specialAbility(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::toggleDebug(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::fullscreen(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::switchItem(const GameEvent event) const {
    this->world.getInventory().switchItem(static_cast<int>(event.axisValue));
}

void EventProcessor::moveX(const GameEvent event) const {
    const float xFactor = event.axisValue;
    float_type speed = NORMAL_SPEED;
    if (this->world.getHiker().getHikerMovement().getState() == HikerMovement::CROUCHED) {
        speed *= DUCK_SPEED_FACTOR;
    }

    auto vel = this->world.getHiker().getVelocity();
    vel.x = speed * xFactor;
    this->world.getHiker().setVelocity(vel);
}

void EventProcessor::moveY(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::noEvent(const GameEvent event) const {
    // Do nothing
}