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
    Game(GameManager *m);
    ~Game();

    bool init(const char *host=NULL);
    void handleEvent(SDL_Event& event);
    void update(int delta);
    void render();

 private:
    Entity player, opponent, ball;
    int score1, score2;
    TCPsocket server;
    SDLNet_SocketSet socketSet;
    bool multiplayer;

    bool netWait();
    void handleInput(int delta);
};

#endif
