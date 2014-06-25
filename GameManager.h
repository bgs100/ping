// -*- c++ -*-
#ifndef PING_GAME_MANAGER_H
#define PING_GAME_MANAGER_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "TitleScreen.h"
#include "Game.h"

class GameManager {
public:
    static const int WIDTH = 1024;
    static const int HEIGHT = 768;

    SDL_Window *window;
    SDL_Renderer *renderer;
    Texture *background;
    TTF_Font *font16, *font24, *font32, *font48, *font64;
    Mix_Chunk *bounceSound, *hitSound;
    bool running;

    GameState *getState();
    void pushState(GameState *state);
    GameState *popState();
    void revertState();
    int run();

private:
    std::vector<GameState *> stateStack;

    bool init();
    void handleEvents();
    void render(double lag);
    void cleanup();
};

#endif
