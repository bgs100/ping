// -*- c++ -*-
#ifndef PING_AI_INPUT_H
#define PING_AI_INPUT_H

#include "PaddleInput.h"
#include "Game.h"

class AIInput: public PaddleInput {
public:
    AIInput(Game *game);
    int update(Entity& paddle);
private:
    Game *game;
};

#endif
