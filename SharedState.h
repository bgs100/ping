// -*- c++ -*-
#ifndef PING_SHARED_STATE_H
#define PING_SHARED_STATE_H

#include <vector>
#include "StateListener.h"
#include "Entity.h"

class SharedState {
public:
    std::vector<Vector2> boundaries;
    std::vector<Entity> players;
    std::vector<int> scores;
    int playerBoundaryOffset;
    Entity ball;
    int score1, score2;
    StateListener *listener;
    int collided;

    SharedState(int numPlayers=0, StateListener *listener=NULL);
    void reset();
    void update(std::vector<int> inputs);

private:
    double centerY;
    double scale;
};

#endif
