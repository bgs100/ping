// -*- c++ -*-
#ifndef PING_GAME_H
#define PING_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "GameState.h"
#include "StateListener.h"
#include "Texture.h"
#include "SharedState.h"
#include "PaddleInput.h"
#include "Socket.h"

class Game: public GameState, public StateListener {
public:
    Game(GameManager *m, PaddleInput *p1input, PaddleInput *p2input, bool demo=false);
    Game(GameManager *m, PaddleInput *p1input, const char *host);
    ~Game();

    void onBounce();
    void onHit();
    void update();
    void render(double lag);

private:
    static Texture whiteTexture;

    SharedState state;
    PaddleInput *playerInput, *opponentInput;
    Socket *server;
    bool networked;
    int playerNum;
    bool demo;

    void setupTextures();
    void errorScreen(const char *msg);
    void handleInput();
};

#endif
