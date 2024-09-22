//
// Created by Anietta Weckauff on 11.07.24.
//

#ifndef SURVIVING_SARNTAL_MENUEVENTPROCESSOR_H
#define SURVIVING_SARNTAL_MENUEVENTPROCESSOR_H

#include "EventProcessor.h"
#include "MenuEngine.h"
#include "MenuRenderer.h"
class MenuEventProcessor : public EventProcessor {

    // using GameEventFunction = void (MenuEventProcessor::*)(GameEvent) const;

  public:
    explicit MenuEventProcessor(MenuEngine &menuEngine);

  private:
    MenuEngine &menuEngine;
    // MenuRenderer &menuRenderer;
    //  std::map<GameEvent, GameEventFunction, GameEventCompare> gameEventFunctionMappings;

    void selectButton(GameEvent event) const;

    void switchButton(GameEvent event) const;

    void noEvent(GameEvent event) const;

    // void startGameplay(GameEvent event) const;

    // void goToStartScreen(GameEvent event) const;

    // void goToSettings(GameEvent event) const;

    // void goToInstructions(GameEvent event) const;
};

#endif // SURVIVING_SARNTAL_MENUEVENTPROCESSOR_H
