// -*- c++ -*-
#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "GameState.h"
#include "GameManager.h"
#include "Entity.h"

class Game: public GameState {
 public:
    Game(GameManager *m, const char *host);
    virtual bool init();
    virtual void handleInput();
    virtual void update();
    virtual void render();

 private:
    Entity player, opponent, ball;
    int score1, score2;
    TCPsocket server;
    SDLNet_SocketSet socketSet;
    const char *host;

    bool netWait();
};

#endif
