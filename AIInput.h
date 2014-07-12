// -*- c++ -*-
#ifndef PING_AI_INPUT_H
#define PING_AI_INPUT_H

#include "PaddleInput.h"

class AIInput: public PaddleInput {
public:
    enum Difficulty { EASY, MEDIUM, HARD, NOPE, INSANITY, NUM_DIFFICULTY };
    static const char *DIFFICULTY_STRS[];

    Difficulty difficulty;

    AIInput(Difficulty difficulty);
    int update(SharedState &state, int player);
};

#endif
