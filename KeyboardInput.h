// -*- c++ -*-
#ifndef PING_KEYBOARD_INPUT_H
#define PING_KEYBOARD_INPUT_H

#include <SDL2/SDL.h>
#include "PaddleInput.h"

class KeyboardInput: public PaddleInput {
public:
    SDL_Scancode upKey, downKey;

    KeyboardInput(SDL_Scancode up, SDL_Scancode down);
    int update(SharedState &state, int player);
};

#endif
