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

    SharedState(StateListener *listener=NULL);
    SharedState(int numPlayers, StateListener *listener=NULL);

    std::vector<Entity *> getEntities();

    void reset(int numPlayers);
    void update(std::vector<int> inputs);

private:
    double centerY;
    double scale;
};

#endif
