// -*- c++ -*-
#ifndef PING_GAME_H
#define PING_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "GameState.h"
#include "GameManager.h"
#include "Entity.h"
#include "PaddleInput.h"

class Game: public GameState {
public:
    Entity ball, player, opponent;

    Game(GameManager *m);
    ~Game();

    bool init(PaddleInput *p1input, PaddleInput *p2input);
    bool init(PaddleInput *p1input, const char *host);
    void handleEvent(SDL_Event& event);
    void update(int delta);
    void render();

private:
    PaddleInput *playerInput, *opponentInput;
    int score1, score2;
    TCPsocket server;
    SDLNet_SocketSet socketSet;
    bool networked;

    bool netWait();
    void handleInput(int delta);
};

#endif
