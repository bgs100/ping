// -*- c++ -*-
#ifndef PING_GAME_H
#define PING_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "GameState.h"
#include "StateListener.h"
#include "SharedState.h"
#include "PaddleInput.h"
#include "Socket.h"

class Game: public GameState, public StateListener {
public:
    Game(GameManager *m);
    ~Game();

    bool init(PaddleInput *p1input, PaddleInput *p2input);
    bool init(PaddleInput *p1input, const char *host);
    void handleEvent(SDL_Event& event);
    void onBounce();
    void onHit();
    void update();
    void render(double lag);

private:
    SharedState state;
    PaddleInput *playerInput, *opponentInput;
    Socket *server;
    bool networked;
    int playerNum;

    bool netWait();
    void handleInput();
};

#endif
