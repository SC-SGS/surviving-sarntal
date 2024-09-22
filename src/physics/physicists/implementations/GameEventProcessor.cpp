//
// Created by bjoern on 5/21/24.
//

#include "../GameEventProcessor.hpp"
#include "../../../game/Game.hpp"

#include <iostream>
#include <mutex>

GameEventProcessor::GameEventProcessor(World &world, Renderer &renderer, HikerConstants &hikerConstants,
                                       MenuEngine &menuEngine)
    : world(world), renderer(renderer), menuEngine(menuEngine), hikerConstants(hikerConstants) {
    this->gameEventFunctionMappings = {
        {{AXIS_MODIFICATION, ITEM_SWITCH, 0, false},
         [this](GameEvent gameEvent) { return this->switchItem(gameEvent); }},
        {{AXIS_MODIFICATION, MOVEMENT_X, 0, false}, [this](GameEvent gameEvent) { return this->moveX(gameEvent); }},
        {{AXIS_MODIFICATION, MOVEMENT_Y, 0, false}, [this](GameEvent gameEvent) { return this->moveY(gameEvent); }},
        {{JUMP, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->jump(gameEvent); }},
        {{CROUCH, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->crouch(gameEvent); }},
        {{UNCROUCH, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->uncrouch(gameEvent); }},
        {{ITEM_PICK, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->pickItem(gameEvent); }},
        {{ITEM_USE, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->useItem(gameEvent); }},
        {{ITEM_DROP, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->dropItem(gameEvent); }},
        {{PAUSE, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->pause(gameEvent); }},
        {{FULLSCREEN, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->fullscreen(gameEvent); }},
        {{SPECIAL_ABILITY, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->specialAbility(gameEvent); }},
        {{TOGGLE_DEBUG, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->toggleDebug(gameEvent); }},
        {{SELECT_BUTTON, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->noEvent(gameEvent); }},
        {{AXIS_MODIFICATION, BUTTON_SWITCH, 0, false},
         [this](GameEvent gameEvent) { return this->noEvent(gameEvent); }},
        {{NO_EVENT, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->noEvent(gameEvent); }}};
    this->autoEventFunctions = {[this]() { return this->pickAutoCollectableItems(); }};
}

/**void GameEventProcessor::processEvents() {
    const auto numEvents = this->eventQueue.size();
    for (int i = 0; i < numEvents; i++) {
        const auto &event = this->eventQueue.front();
        // spdlog::info("Event with axis value {}.", event.axisValue);
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
}**/

void GameEventProcessor::crouch(const GameEvent event) const {
    // std::cout << "Crouch hiker." << std::endl;
    this->world.getHiker().crouch();
}

void GameEventProcessor::uncrouch(const GameEvent event) const {
    // std::cout << "Uncrouch hiker." << std::endl;
    this->world.getHiker().uncrouch();
}

void GameEventProcessor::pickItem(const GameEvent event) const {
    const auto &items = this->world.getNearbyItems();
    for (auto const &item : items) {
        if (!item->canAutoCollect()) {
            pickItem(item);
        }
    }
}

void GameEventProcessor::pickAutoCollectableItems() const {
    const auto &items = this->world.getNearbyItems();
    for (auto const &item : items) {
        if (item->canAutoCollect()) {
            pickItem(item);
        }
    }
}

void GameEventProcessor::dropItem(GameEvent event) const { this->world.getInventory().removeSelectedItem(); }

void GameEventProcessor::useItem(const GameEvent event) const {
    if (!this->world.getInventory().selectedSlotIsEmpty()) {
        this->world.useItem(this->world.getInventory().getSelectedItemType());
        this->world.getInventory().removeSelectedItem();
    }
}

void GameEventProcessor::jump(GameEvent event) const { this->world.getHiker().jump(); }

void GameEventProcessor::pause(const GameEvent event) {
    this->menuEngine.setGameplayRunning(false);
    this->menuEngine.switchTo(PAUSE_SCREEN);
}

void GameEventProcessor::specialAbility(const GameEvent event) const {
    // TODO: implement
}

void GameEventProcessor::toggleDebug(const GameEvent event) const { // NOLINT(*-convert-member-functions-to-static)
    this->renderer.toggleDebugMode();
}

void GameEventProcessor::fullscreen(const GameEvent event) const {
    // TODO: implement
}

void GameEventProcessor::switchItem(const GameEvent event) const {
    this->world.getInventory().switchItemSlot(static_cast<int>(event.axisValue));
}

void GameEventProcessor::moveX(const GameEvent event) const {
    const floatType xFactor = event.axisValue;
    floatType speed = hikerConstants.normalSpeed;
    if (this->world.getHiker().getHikerMovement().getState() == HikerMovement::CROUCHED) {
        speed *= hikerConstants.duckSpeedFactor;
    }
    this->world.getHiker().setXVelocity(speed * xFactor);
}

void GameEventProcessor::moveY(const GameEvent event) const {
    // TODO: implement
}

void GameEventProcessor::noEvent(const GameEvent event) const {
    // Do nothing
}

/**void GameEventProcessor::addEvents(std::queue<GameEvent> &eventQueue) {
    while (!eventQueue.empty()) {
        this->eventQueue.push(eventQueue.front());
        eventQueue.pop();
    }
}

void GameEventProcessor::clearRepeatedEvents() {
    std::queue<GameEvent> newEvents = {};
    while (!this->eventQueue.empty()) {
        GameEvent event = this->eventQueue.front();
        this->eventQueue.pop();
        if (!event.executeRepeatedly) {
            newEvents.push(event);
        }
    }
    this->eventQueue = newEvents;
}**/

void GameEventProcessor::pickItem(const std::shared_ptr<Item> &item) const {
    if (item->canUseOnPickUp()) {
        this->world.useItem(item->getItemType());
        this->world.getItems().remove(item);
    } else if (this->world.getInventory().canCollectItem(item)) {
        this->world.getInventory().addItem(item);
        this->world.getItems().remove(item);
    }
}
