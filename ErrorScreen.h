// -*- c++ -*-
#ifndef PING_ERROR_SCREEN_H
#define PING_ERROR_SCREEN_H

#include "GameState.h"
#include "Texture.h"

class ErrorScreen: public GameState {
public:
    ErrorScreen(GameManager *m, const char *msg);
    ~ErrorScreen();

    void handleEvent(SDL_Event &event);
    void render();

private:
    static Texture *errorText, *continueText;
    Texture *text;
};

#endif
