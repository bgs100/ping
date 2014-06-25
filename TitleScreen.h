// -*- c++ -*-
#ifndef PING_TITLE_SCREEN_H
#define PING_TITLE_SCREEN_H

#include <SDL2/SDL.h>
#include "GameState.h"
#include "Texture.h"
#include "ButtonMenu.h"

class TitleScreen: public GameState {
public:
    TitleScreen(GameManager *m);
    ~TitleScreen();
    void handleEvent(SDL_Event &event);
    void render();

private:
    enum Button { SINGLEPLAYER, MULTIPLAYER_LOCAL, MULTIPLAYER_NET, TUTORIAL, CREDITS, QUIT, END_BUTTON };
    static const char *labels[];

    Texture *titleText;
    ButtonMenu buttonMenu;
};

#endif
