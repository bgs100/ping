// -*- c++ -*-
#ifndef PING_PADDLE_INPUT_H
#define PING_PADDLE_INPUT_H

#include "Entity.h"

class PaddleInput {
public:
    virtual ~PaddleInput() {}
    virtual int update(Entity& paddle) = 0;
};

#endif
