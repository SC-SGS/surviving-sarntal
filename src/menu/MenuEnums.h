//
// Created by Anietta Weckauff on 05.08.24.
//

#ifndef SURVIVING_SARNTAL_MENUENUMS_H
#define SURVIVING_SARNTAL_MENUENUMS_H

typedef enum ScreenState {
    START_SCREEN,
    SETTINGS_SCREEN,
    INSTRUCTIONS_SCREEN,
    END_SCREEN,
    PAUSE_SCREEN,
    NO_MENU_SCREEN,
    NO_SCREEN
} ScreenState;

typedef enum ButtonType {
    START_GAME,
    SETTINGS,
    INSTRUCTIONS,
    CONTINUE_GAME,
    HOME,
    PLAY_AGAIN,
    BACK_TO_MENU,
    CLOSE_GAME
} ButtonType;

#endif // SURVIVING_SARNTAL_MENUENUMS_H
