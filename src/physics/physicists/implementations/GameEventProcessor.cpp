//
// Created by bjoern on 5/21/24.
//

#include "../GameEventProcessor.hpp"
#include "../../../game/Game.hpp"

#include <iostream>
#include <mutex>

GameEventProcessor::GameEventProcessor(World &world,
                                       Renderer &renderer,
                                       const GameConstants &gameConstants,
                                       MenuEngine &menuEngine,
                                       AudioService &audioService)
    : world(world),
      renderer(renderer),
      menuEngine(menuEngine),
      gameConstants(gameConstants),
      audioService(audioService) {
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

void GameEventProcessor::crouch(const GameEvent event) const {
    // std::cout << "Crouch hiker." << std::endl;
    this->world.getHiker().crouch();
}

void GameEventProcessor::uncrouch(const GameEvent event) const {
    // std::cout << "Uncrouch hiker." << std::endl;
    // this->world.getHiker().uncrouch();
    this->world.getHiker().setShouldUncrouch();
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

void GameEventProcessor::useItem(const GameEvent event) const { this->world.useSelectedItem(); }

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
    // TODO: move here
}

void GameEventProcessor::switchItem(const GameEvent event) const {
    this->world.getInventory().switchItemSlot(static_cast<int>(event.axisValue));
}

void GameEventProcessor::moveX(const GameEvent event) {
    const floatType xFactor = event.axisValue;
    const floatType speed = this->calculateHikerSpeed(xFactor);
    const HikerMovement::Direction oldDir = this->world.getHiker().getHikerMovement().getDirection();
    this->world.getHiker().setXVelocity(speed);
    if (fabsf(speed) < NUMERIC_EPSILON || oldDir * this->world.getHiker().getHikerMovement().getDirection() < 0) {
        this->audioService.interruptMovingSound();
        return;
    }
    if (this->world.getHiker().getHikerMovement().getState() != HikerMovement::IN_AIR) {
        this->audioService.playMovingSound(speed, this->gameConstants.hikerConstants.maxSpeed);
    }
}

void GameEventProcessor::moveY(const GameEvent event) const {}

void GameEventProcessor::noEvent(const GameEvent event) const {
    // Do nothing
}

void GameEventProcessor::pickItem(const std::shared_ptr<Item> &item) const {
    if (item->canUseOnPickUp()) {
        this->world.useItem(item->getItemType());
        this->world.getItems().remove(item);
    } else if (this->world.getInventory().canCollectItem(item)) {
        this->world.getInventory().addItem(item);
        this->world.getItems().remove(item);
    }
}

floatType GameEventProcessor::calculateHikerSpeed(const floatType factor) const {
    floatType hikerBaseSpeed = this->gameConstants.hikerConstants.normalSpeed;
    floatType coinSpeedFactor = this->getCoinSpeedFactor();
    floatType movementFactor = this->getHikerMovementFactor();

    floatType monsterSpeed = this->world.getMonster().getVelocity();

    floatType hikerSpeed = fmaxf(hikerBaseSpeed * coinSpeedFactor * movementFactor, monsterSpeed);

    hikerSpeed *= factor;

    if (fabs(hikerSpeed) > this->gameConstants.hikerConstants.maxSpeed) {
        return this->gameConstants.hikerConstants.maxSpeed * factor / fabsf(factor);
    } else {
        return hikerSpeed;
    }
}

floatType GameEventProcessor::getCoinSpeedFactor() const {
    floatType numberOfCoins = static_cast<floatType>(this->world.getCoinScore()) /
                              static_cast<floatType>(gameConstants.itemsConstants.coinScore);
    floatType factor = 1.0f + numberOfCoins * gameConstants.itemsConstants.coinAccelerationFactor;
    return factor;
}

floatType GameEventProcessor::getHikerMovementFactor() const {
    const Hiker &hiker = this->world.getHiker();
    if (hiker.getHikerMovement().getState() == HikerMovement::CROUCHED) {
        return this->gameConstants.hikerConstants.duckSpeedFactor;
    } else if (hiker.getHikerMovement().getState() == HikerMovement::IN_AIR) {
        return this->gameConstants.hikerConstants.airMovementSpeedFactor;
    } else {
        return 1.0f;
    }
}
