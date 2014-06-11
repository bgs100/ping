// -*- c++ -*-
#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include <SDL2/SDL.h>
#include "GameState.h"

class TitleScreen: public GameState {
public:
    TitleScreen(GameManager *m) : GameState(m) {}
    virtual bool init();
    virtual void handleEvent(SDL_Event &event);
    virtual void handleInput();
    virtual void render();

private:
    SDL_Texture *titleText, *playText;
    int titleW, titleH, playW, playH;
};

#endif
