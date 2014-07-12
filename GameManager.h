// -*- c++ -*-
#ifndef PING_GAME_MANAGER_H
#define PING_GAME_MANAGER_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "TitleScreen.h"
#include "Game.h"

enum Font { FONT_RND, FONT_SQR, FONT_END };
enum FontSize { SIZE_8, SIZE_12, SIZE_16, SIZE_24, SIZE_32, SIZE_48, SIZE_64, SIZE_END };

class GameManager {
public:
    static const int WIDTH = 1024;
    static const int HEIGHT = 768;

    static const int fontSizes[];

    SDL_Window *window;
    SDL_Renderer *renderer;
    Texture background;
    TTF_Font *fonts[FONT_END][SIZE_END];
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
