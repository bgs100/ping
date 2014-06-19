// -*- c++ -*-
#ifndef PING_AI_INPUT_H
#define PING_AI_INPUT_H

#include "PaddleInput.h"
#include "Game.h"

class AIInput: public PaddleInput {
public:
    enum Difficulty { MEDIUM, HARD, NOPE, INSANITY };

    AIInput(Game *game, Difficulty difficulty);
    int update(Entity& paddle);
private:
    Game *game;
    Difficulty difficulty;
};

#endif
