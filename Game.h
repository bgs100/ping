// -*- c++ -*-
#ifndef PING_GAME_H
#define PING_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "GameState.h"
#include "GameManager.h"
#include "Entity.h"

class Game: public GameState {
 public:
    const char *host;

    Game(GameManager *m, const char *host);
    virtual bool init();
    virtual void update(int delta);
    virtual void render();

 private:
    Entity player, opponent, ball;
    int score1, score2;
    TCPsocket server;
    SDLNet_SocketSet socketSet;

    bool netWait();
    void handleInput(int delta);
};

#endif
