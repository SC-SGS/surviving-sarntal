//
// Created by bjoern on 5/21/24.
//

#include "../EventProcessor.hpp"

#include <iostream>
#include <mutex>

EventProcessor::EventProcessor() : world(World::getInstance()) {
    gameEventFunctionMappings = {
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
    autoEventFunctions = {
        &EventProcessor::pickAutoCollectableItems,
    };
    std::cout << "EventProcessor gets constructed" << std::endl;
}

EventProcessor::~EventProcessor() { std::cout << "EventProcessor gets deconstructed" << std::endl; }

void EventProcessor::processEvents() {
    const auto numEvents = this->eventQueue.size();
    for (int i = 0; i < numEvents; i++) {
        const auto &event = this->eventQueue.front();
        // std::cout << "Game event: " << event.type << std::endl;
        //  std::cout << "Game event type (EventProcessor): " << event.type << std::endl;
        //  std::cout << "Game event axis (EventProcessor): " << event.axis << std::endl;
        const GameEventFunction func = gameEventFunctionMappings.at(event);
        // std::cout << "Function exists? " << (func != nullptr) << std::endl;
        (this->*func)(event);
        // TODO remove one-time events like jump from queue, but add hold events (especially walk) again?
        this->eventQueue.pop();
    }
    for (auto eventFunction : this->autoEventFunctions) {
        (this->*eventFunction)();
    }
}

// TODO: make hiker methods
void EventProcessor::crouch(const GameEvent event) const {
    // std::cout << "Crouch hiker." << std::endl;
    auto &hiker = this->world.getHiker();
    auto &hikerMovement = this->world.getHiker().getHikerMovement();
    if (hikerMovement.getState() == HikerMovement::MOVING) {
        hikerMovement.setState(HikerMovement::CROUCHED);
        hiker.setHeight(DUCKED_HIKER_HEIGHT);
        hiker.setWidth(DUCKED_HIKER_WIDTH);
    }
}

// TODO: sounds
void EventProcessor::uncrouch(const GameEvent event) const {
    // std::cout << "Uncrouch hiker." << std::endl;
    auto &hiker = this->world.getHiker();
    auto &hikerMovement = this->world.getHiker().getHikerMovement();
    if (hikerMovement.getState() == HikerMovement::CROUCHED) {
        hikerMovement.setState(HikerMovement::MOVING);
        hiker.setHeight(HIKER_HEIGHT);
        hiker.setWidth(HIKER_WIDTH);
    }
}

void EventProcessor::pickItem(const GameEvent event) const {
    const auto &items = this->world.getNearbyItems();
    for (auto const &item : items) {
        const ItemInformation info = Item::getItemInformation(item->getItemType());
        if (!info.autoCollect) {
            if (!info.useOnPickup) {
                this->inventoryHandler.pickUpItem(item);
            } else {
                this->world.useItem(item->getItemType());
            }
        }
    }
}

void EventProcessor::pickAutoCollectableItems() const {
    const auto &items = this->world.getNearbyItems();
    for (auto const &item : items) {
        const ItemInformation info = Item::getItemInformation(item->getItemType());
        if (info.autoCollect) {
            if (!info.useOnPickup) {
                this->inventoryHandler.pickUpItem(item);
            } else {
                this->world.useItem(item->getItemType());
            }
        }
    }
}

void EventProcessor::dropItem(GameEvent event) const {
    if (!this->world.getInventory().isSelectedSlotFree()) {
        this->inventoryHandler.removeSelectedItem();
    }
}

void EventProcessor::useItem(const GameEvent event) const {
    if (!this->world.getInventory().isSelectedSlotFree()) {
        this->world.useItem(this->world.getInventory().getSelectedItem());
        this->inventoryHandler.removeSelectedItem();
    }
}

void EventProcessor::jump(GameEvent event) const {
    // std::cout << "Jump hiker." << std::endl;
    auto &hiker = this->world.getHiker();
    auto &hikerMovement = hiker.getHikerMovement();
    if (hikerMovement.getState() == HikerMovement::CROUCHED) {
        return;
    }
    if (hikerMovement.getState() != HikerMovement::IN_AIR) {
        hikerMovement.setLastJump(0.0);
        hikerMovement.setCanJumpAgain(true);
    }
    if (hikerMovement.getLastJump() < 1.5 && hikerMovement.getCanJumpAgain()) {
        auto &vel = hiker.getVelocity();
        vel.y = JUMP_VELOCITY_CONSTANT;
        hiker.setVelocity(vel);
        if (hikerMovement.getState() == HikerMovement::IN_AIR) {
            hikerMovement.setCanJumpAgain(false);
        }
        hikerMovement.setState(HikerMovement::IN_AIR);
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
    floatType speed = NORMAL_SPEED;
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

void EventProcessor::setEventQueue(std::queue<GameEvent> &eventQueue) { this->eventQueue = eventQueue; }
