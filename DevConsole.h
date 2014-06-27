// -*- c++ -*-
#ifndef PING_DEV_CONSOLE_H
#define PING_DEV_CONSOLE_H

#include <string>
#include "GameState.h"
#include "Textbox.h"

class DevConsole: public GameState {
public:
    DevConsole(GameManager *m);

    void handleEvent(SDL_Event &event);
    void render();

private:
    Textbox commandInput;

    bool handleCommand(std::string input);
};

#endif
