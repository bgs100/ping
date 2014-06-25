// -*- c++ -*-
#ifndef PING_DIFFICULTY_MENU_H
#define PING_DIFFICULTY_MENU_H

#include "GameState.h"
#include "ButtonMenu.h"

class DifficultyMenu: public GameState {
public:
    DifficultyMenu(GameManager *m);

    void handleEvent(SDL_Event &event);
    void render();

private:
    enum Button { EASY, MEDIUM, HARD, NOPE, INSANITY, END_BUTTON };
    static const char *labels[];

    ButtonMenu buttonMenu;
};

#endif
