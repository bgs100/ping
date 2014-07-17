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
    StateListener *listener;
    int collided;
    double centerY, scale;

    SharedState(StateListener *listener=NULL);
    SharedState(int numPlayers, int wallsPerPlayer, StateListener *listener=NULL);

    std::vector<Entity *> getEntities();

    void resetBall();
    void resetClassic();
    void reset(int numPlayers, int wallMult);
    void update(std::vector<int> inputs);

    int playerToBoundaryIndex(int playerIndex) const;
    int boundaryToPlayerIndex(int boundaryIndex) const;
};

#endif
