// -*- c++ -*-
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "TitleScreen.h"
#include "Game.h"

class GameManager {
public:
    static const int WIDTH = 1024;
    static const int HEIGHT = 768;
    GameManager(const char *host=NULL);
    int run();

    SDL_Texture *textureText(TTF_Font *font, const char *str, Uint8 r, Uint8 g, Uint8 b, int *w=NULL, int *h=NULL);

    TitleScreen titleScreen;
    Game game;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    TTF_Font *font32, *font48, *font64;
    const char *host;
    GameState *state;

    bool running;

private:
    bool init();
    void handleInput();
    void render();
};

#endif
