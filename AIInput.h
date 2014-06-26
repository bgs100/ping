// -*- c++ -*-
#ifndef PING_AI_INPUT_H
#define PING_AI_INPUT_H

#include "PaddleInput.h"

class AIInput: public PaddleInput {
public:
    enum Difficulty { EASY, MEDIUM, HARD, NOPE, INSANITY, NUM_DIFFICULTY };

    AIInput(Difficulty difficulty);
    int update(SharedState &state, int player);
private:
    Difficulty difficulty;
};

#endif
