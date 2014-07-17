// -*- c++ -*-
#ifndef PING_GAME_H
#define PING_GAME_H

#include <vector>
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
    Game(GameManager *m, std::vector<PaddleInput *> inputs, int wallsPerPlayer, bool classic=false, bool demo=false);
    Game(GameManager *m, PaddleInput *input, const char *host);
    ~Game();

    void onBounce();
    void onHit();
    void update();
    void render(double lag);

private:
    static Texture whiteTexture;

    Texture background, overlay;
    SharedState state;
    std::vector<PaddleInput *> inputs;
    Socket *server;
    bool networked;
    int playerNum;
    bool classic, demo;

    void setupStatic();
    void setupTextures();
    void errorScreen(const char *msg);
    void handleInput();
};

#endif
