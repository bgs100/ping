// -*- c++ -*-
#ifndef PING_TITLE_SCREEN_H
#define PING_TITLE_SCREEN_H

#include <SDL2/SDL.h>
#include "GameState.h"
#include "Texture.h"

class TitleScreen: public GameState {
public:
    TitleScreen(GameManager *m) : GameState(m) {}
    virtual bool init();
    virtual void handleEvent(SDL_Event &event);
    virtual void handleInput();
    virtual void render();

private:
    Texture *titleText, *playText;
};

#endif
