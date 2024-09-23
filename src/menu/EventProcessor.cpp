//
// Created by Anietta Weckauff on 11.07.24.
//

#include "EventProcessor.h"

EventProcessor::EventProcessor() = default;

void EventProcessor::processEvents() {
    const auto numEvents = this->eventQueue.size();
    for (int i = 0; i < numEvents; i++) {
        const auto &event = this->eventQueue.front();
        // spdlog::info("Event with axis value {}.", event.axisValue);
        auto iterator = this->gameEventFunctionMappings.find(event);
        if (iterator != this->gameEventFunctionMappings.end()) {
            const std::function<void(GameEvent)> func = gameEventFunctionMappings.at(event);
            func(event);
        }

        if (event.executeRepeatedly) {
            this->eventQueue.push(event);
        }
        this->eventQueue.pop();
    }
    for (const auto &eventFunction : this->autoEventFunctions) {
        eventFunction();
    }
}

void EventProcessor::addEvents(std::queue<GameEvent> &eventQueue1) {
    while (!eventQueue1.empty()) {
        this->eventQueue.push(eventQueue1.front());
        eventQueue1.pop();
    }
}

void EventProcessor::clearRepeatedEvents() {
    std::queue<GameEvent> newEvents = {};
    while (!this->eventQueue.empty()) {
        GameEvent event = this->eventQueue.front();
        this->eventQueue.pop();
        if (!event.executeRepeatedly) {
            newEvents.push(event);
        }
    }
    this->eventQueue = newEvents;
}
