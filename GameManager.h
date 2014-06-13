// -*- c++ -*-
#ifndef PING_GAME_MANAGER_H
#define PING_GAME_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "TitleScreen.h"
#include "Game.h"

class GameManager {
public:
    static const int WIDTH = 1024;
    static const int HEIGHT = 768;
    GameManager(const char *host=NULL);
    int run();

    TitleScreen titleScreen;
    Game game;

    SDL_Window *window;
    SDL_Renderer *renderer;
    Texture *background;
    TTF_Font *font16, *font24, *font32, *font48, *font64;
    Mix_Chunk *bounceSound, *hitSound;
    const char *host;
    GameState *state;

    bool running;

private:
    bool init();
    void handleInput();
    void render();
};

#endif
