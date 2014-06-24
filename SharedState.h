// -*- c++ -*-
#ifndef PING_SHARED_STATE_H
#define PING_SHARED_STATE_H

#include "StateListener.h"
#include "Entity.h"

class SharedState {
public:
    Entity player, opponent, ball;
    int score1, score2;
    StateListener *listener;
    bool collided;

    SharedState(StateListener *listener=NULL);
    void reset();
    void update(int pInput, int oInput);
};

#endif
