// -*- c++ -*-
#ifndef PING_TITLE_SCREEN_H
#define PING_TITLE_SCREEN_H

#include <SDL2/SDL.h>
#include "GameState.h"
#include "Texture.h"
#include "MultiplayerMenu.h"

enum Button { MULTIPLAYER_LOCAL, MULTIPLAYER_NET, TUTORIAL, CREDITS, QUIT, END_BUTTON };

class TitleScreen: public GameState {
public:
    TitleScreen(GameManager *m);
    virtual bool init();
    virtual void handleEvent(SDL_Event &event);
    virtual void handleInput();
    virtual void render();
    void cleanup();

private:
    MultiplayerMenu multiplayerMenu;
    Texture *titleText;
    Texture *selectedButtons[END_BUTTON];
    Texture *unselectedButtons[END_BUTTON];
    int selected;

    int getButton(int x, int y);
};

#endif
