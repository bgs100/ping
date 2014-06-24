// -*- c++ -*-
#ifndef PING_PADDLE_INPUT_H
#define PING_PADDLE_INPUT_H

#include "SharedState.h"

class PaddleInput {
public:
    virtual ~PaddleInput() {}
    virtual int update(SharedState &state, int player) = 0;
};

#endif
