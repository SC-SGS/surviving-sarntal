//
// Created by Anietta Weckauff on 11.07.24.
//

#ifndef SURVIVING_SARNTAL_EVENTPROCESSOR_H
#define SURVIVING_SARNTAL_EVENTPROCESSOR_H

#include "../input/InputHandler.h"
#include "../input/events/GameEvent.h"

class EventProcessor {

  public:
    explicit EventProcessor();
    ~EventProcessor() = default;
    /**
     * Performs the changes to the world caused by the user input.
     */
    void processEvents();

    /**
     * Adds new events to the queue.
     *
     * @param eventQueue
     */
    void addEvents(std::queue<GameEvent> &eventQueue);

    /**
     * Clears events executed repeatedly from the event queue.
     */
    void clearRepeatedEvents();

  private:
    std::queue<GameEvent> eventQueue{};

  protected:
    // using GameEventFunction = void (EventProcessor::*)(GameEvent) const;
    // using AutoEventFunction = void (EventProcessor::*)() const;
    std::map<GameEvent, std::function<void(GameEvent)>, GameEventCompare> gameEventFunctionMappings;
    std::list<std::function<void()>> autoEventFunctions;
};

#endif // SURVIVING_SARNTAL_EVENTPROCESSOR_H
