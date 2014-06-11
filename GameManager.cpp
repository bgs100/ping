#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "GameManager.h"
#include "utility.h"

GameManager::GameManager(const char *host) : game(this, host), host(host) {}

bool GameManager::init() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return SDLerror("SDL_Init");

    if (TTF_Init() != 0)
        return SDLerror("TTF_Init");

    if (host != NULL && SDLNet_Init() != 0) {
        netError("SDLNet_Init");
        host = NULL;
    }

    window = SDL_CreateWindow("PiNG", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
	return SDLerror("SDL_CreateWindow");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
        return SDLerror("SDL_CreateRenderer");

    SDL_Surface *tmp = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    if (tmp == NULL)
        return SDLerror("SDL_CreateRGBSurface");

    SDL_Rect line = { WIDTH/2 - 6, 0, 12, HEIGHT };
    SDL_FillRect(tmp, &line, SDL_MapRGB(tmp->format, 0xff, 0xff, 0xff));
    background = SDL_CreateTextureFromSurface(renderer, tmp);
    if (background == NULL)
        return SDLerror("SDL_CreateTextureFromSurface");
    SDL_FreeSurface(tmp);

    font = TTF_OpenFont("kenpixel.ttf", 48);
    if (font == NULL)
        return SDLerror("TTF_OpenFont");

    game.init();
    state = &game;

    return true;
}

SDL_Texture *GameManager::textureText(const char *str, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Color color = { r, g, b, 0xff };
    SDL_Surface *text = TTF_RenderText_Solid(font, str, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text);
    SDL_FreeSurface(text);
    return texture;
}

void GameManager::handleInput() {
    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        else
            state->handleEvent(event);
    }

    state->handleInput();
}

int GameManager::run() {
    running = true;

    if (!init())
        return 1;

    while (running) {
        handleInput();
        state->update();
        state->render();
    }

    return 0;
}

int main(int argc, char **argv) {
    GameManager *gm;
    if (argc > 1)
        gm = new GameManager(argv[1]);
    else
        gm = new GameManager();
    return gm->run();
}
