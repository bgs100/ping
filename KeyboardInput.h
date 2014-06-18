// -*- c++ -*-
#ifndef PING_KEYBOARD_INPUT_H
#define PING_KEYBOARD_INPUT_H

#include <SDL2/SDL.h>
#include "PaddleInput.h"

class KeyboardInput: public PaddleInput {
public:
    KeyboardInput(SDL_Scancode up, SDL_Scancode down);
    int update(Entity& paddle);

private:
    SDL_Scancode upKey, downKey;
};

#endif
