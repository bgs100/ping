// -*- c++ -*-
#ifndef PING_DEV_CONSOLE_H
#define PING_DEV_CONSOLE_H

#include "GameState.h"
#include "Texture.h"

class DevConsole: public GameState {
public:
    DevConsole(GameManager *m);
    ~DevConsole();

    void handleEvent(SDL_Event &event);
    void render();

private:
    char inputText[512];
    Texture *inputTexture;

    bool handleCommand();
};

#endif
