//
// Created by bjoern on 5/21/24.
//

#include "../EventProcessor.hpp"
#include "../../../game/Game.hpp"

#include <iostream>
#include <mutex>

EventProcessor::EventProcessor() : world(World::getInstance()) {
    gameEventFunctionMappings = {
        {{AXIS_MODIFICATION, ITEM_SWITCH, 0, false}, &EventProcessor::switchItem},
        {{AXIS_MODIFICATION, MOVEMENT_X, 0, false}, &EventProcessor::moveX},
        {{AXIS_MODIFICATION, MOVEMENT_Y, 0, false}, &EventProcessor::moveY},
        {{JUMP, NO_AXIS, 0, false}, &EventProcessor::jump},
        {{CROUCH, NO_AXIS, 0, false}, &EventProcessor::crouch},
        {{UNCROUCH, NO_AXIS, 0, false}, &EventProcessor::uncrouch},
        {{ITEM_PICK, NO_AXIS, 0, false}, &EventProcessor::pickItem},
        {{ITEM_USE, NO_AXIS, 0, false}, &EventProcessor::useItem},
        {{ITEM_DROP, NO_AXIS, 0, false}, &EventProcessor::dropItem},
        {{PAUSE, NO_AXIS, 0, false}, &EventProcessor::pause},
        {{FULLSCREEN, NO_AXIS, 0, false}, &EventProcessor::fullscreen},
        {{SPECIAL_ABILITY, NO_AXIS, 0, false}, &EventProcessor::specialAbility},
        {{TOGGLE_DEBUG, NO_AXIS, 0, false}, &EventProcessor::toggleDebug},
        {{NO_EVENT, NO_AXIS, 0, false}, &EventProcessor::noEvent},
    };
    autoEventFunctions = {
        &EventProcessor::pickAutoCollectableItems,
    };
}

EventProcessor::~EventProcessor() = default;

void EventProcessor::processEvents() {
    const auto numEvents = this->eventQueue.size();
    for (int i = 0; i < numEvents; i++) {
        const auto &event = this->eventQueue.front();
        const GameEventFunction func = gameEventFunctionMappings.at(event);
        (this->*func)(event);
        if (event.executeRepeatedly) {
            this->eventQueue.push(event);
        }
        this->eventQueue.pop();
    }
    for (auto eventFunction : this->autoEventFunctions) {
        (this->*eventFunction)();
    }
}

void EventProcessor::crouch(const GameEvent event) const {
    // std::cout << "Crouch hiker." << std::endl;
    this->world.getHiker().crouch();
}

void EventProcessor::uncrouch(const GameEvent event) const {
    // std::cout << "Uncrouch hiker." << std::endl;
    this->world.getHiker().uncrouch();
}

void EventProcessor::pickItem(const GameEvent event) const {
    const auto &items = this->world.getNearbyItems();
    for (auto const &item : items) {
        if (!item->canAutoCollect()) {
            pickItem(item);
        }
    }
}

void EventProcessor::pickAutoCollectableItems() const {
    const auto &items = this->world.getNearbyItems();
    for (auto const &item : items) {
        if (item->canAutoCollect()) {
            pickItem(item);
        }
    }
}

void EventProcessor::dropItem(GameEvent event) const {
    if (!this->world.getInventory().selectedSlotIsEmpty()) {
        this->world.getInventory().removeSelectedItem();
    }
}

void EventProcessor::useItem(const GameEvent event) const {
    if (!this->world.getInventory().selectedSlotIsEmpty()) {
        this->world.useItem(this->world.getInventory().getSelectedItemType());
        this->world.getInventory().removeSelectedItem();
    }
}

void EventProcessor::jump(GameEvent event) const {
    // std::cout << "Jump hiker." << std::endl;
    this->world.getHiker().jump();
}

void EventProcessor::pause(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::specialAbility(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::toggleDebug(const GameEvent event) const { // NOLINT(*-convert-member-functions-to-static)
    Game::getInstance().debugMode = !Game::getInstance().debugMode;
}

void EventProcessor::fullscreen(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::switchItem(const GameEvent event) const {
    this->world.getInventory().switchItemSlot(static_cast<int>(event.axisValue));
}

void EventProcessor::moveX(const GameEvent event) const {
    const floatType xFactor = event.axisValue;
    floatType speed = NORMAL_SPEED;
    if (this->world.getHiker().getHikerMovement().getState() == HikerMovement::CROUCHED) {
        speed *= DUCK_SPEED_FACTOR;
    }
    this->world.getHiker().setXVelocity(speed * xFactor);
}

void EventProcessor::moveY(const GameEvent event) const {
    // TODO: implement
}

void EventProcessor::noEvent(const GameEvent event) const {
    // Do nothing
}

void EventProcessor::setEventQueue(std::queue<GameEvent> &eventQueue) { this->eventQueue = eventQueue; }

void EventProcessor::pickItem(const std::shared_ptr<Item> &item) const {
    if (item->canUseOnPickUp()) {
        this->world.useItem(item->getItemType());
        this->world.getItems().remove(item);
    } else if (this->world.getInventory().canCollectItem(item)) {
        this->world.getInventory().addItem(item);
        this->world.getItems().remove(item);
    }
}
