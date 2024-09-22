//
// Created by Anietta Weckauff on 11.07.24.
//

#include "MenuEventProcessor.h"

MenuEventProcessor::MenuEventProcessor(MenuEngine &menuEngine) : menuEngine(menuEngine) {
    this->gameEventFunctionMappings = {
        //{{START_GAMEPLAY, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->startGameplay(gameEvent); }},
        //{{SETTINGS, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->goToSettings(gameEvent); }},
        //{{INSTRUCTIONS, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->goToInstructions(gameEvent);
        //}},
        //{{MENU, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->goToStartScreen(gameEvent); }},
        {{AXIS_MODIFICATION, BUTTON_SWITCH, 0, false},
         [this](GameEvent gameEvent) { return this->switchButton(gameEvent); }},
        {{SELECT_BUTTON, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->selectButton(gameEvent); }},
        {{AXIS_MODIFICATION, MOVEMENT_X, 0, false}, [this](GameEvent gameEvent) { return this->noEvent(gameEvent); }},
        {{AXIS_MODIFICATION, MOVEMENT_Y, 0, false}, [this](GameEvent gameEvent) { return this->noEvent(gameEvent); }},
        {{JUMP, NO_AXIS, 0, false}, [this](GameEvent gameEvent) { return this->noEvent(gameEvent); }}};
}

void MenuEventProcessor::selectButton(GameEvent event) const { this->menuEngine.switchScreen(); }
void MenuEventProcessor::switchButton(GameEvent event) const {
    this->menuEngine.switchButton(static_cast<int>(event.axisValue));
}
void MenuEventProcessor::noEvent(GameEvent event) const {
    // do nothing
}
