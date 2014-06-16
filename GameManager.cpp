#include <stdlib.h>
#include <time.h>
#include "GameManager.h"
#include "utility.h"

bool GameManager::init() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return SDLerror("SDL_Init");

    if (TTF_Init() != 0)
        return SDLerror("TTF_Init");

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
        return SDLerror("Mix_OpenAudio");

    if (SDLNet_Init() != 0)
        return SDLerror("SDLNet_Init");
    
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
    background = Texture::fromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    font16 = TTF_OpenFont("kenpixel-square-mod.ttf", 16);
    font24 = TTF_OpenFont("kenpixel-square-mod.ttf", 24);
    font32 = TTF_OpenFont("kenpixel-square-mod.ttf", 32);
    font48 = TTF_OpenFont("kenpixel-square-mod.ttf", 48);
    font64 = TTF_OpenFont("kenpixel-square-mod.ttf", 64);
    if (font16 == NULL || font24 == NULL || font32 == NULL || font48 == NULL || font64 == NULL)
        return SDLerror("TTF_OpenFont");

    bounceSound = Mix_LoadWAV("boop.wav");
    hitSound = Mix_LoadWAV("hit.wav");
    if (bounceSound == NULL || hitSound == NULL)
        return SDLerror("Mix_LoadWAV");

    Mix_AllocateChannels(16);

    TitleScreen *titleScreen = new TitleScreen(this);
    pushState(titleScreen);
    titleScreen->init();

    return true;
}

void GameManager::pushState(GameState *state) {
    stateStack.push_back(state);
}

GameState *GameManager::popState() {
    GameState *top = stateStack.back();
    stateStack.pop_back();
    return top;
}

void GameManager::revertState() {
    delete popState();
}

void GameManager::swapState(GameState *state) {
    revertState();
    pushState(state);
}

void GameManager::handleEvents() {
    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        else
            stateStack.back()->handleEvent(event);
    }
}

void GameManager::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    stateStack.back()->render();

    SDL_RenderPresent(renderer);
}

int GameManager::run() {
    running = true;

    if (!init())
        return 1;

    Uint32 last, time = SDL_GetTicks(), delta = 0;
    while (running) {
        handleEvents();
        stateStack.back()->update(delta);
        render();
        last = time;
        time = SDL_GetTicks();
        delta = time - last;
    }

    cleanup();

    return 0;
}

void GameManager::cleanup() {
    for (std::vector<GameState *>::reverse_iterator it = stateStack.rbegin(); it != stateStack.rend(); it++)
        delete *it;

    Mix_FreeChunk(hitSound);
    Mix_FreeChunk(bounceSound);

    TTF_CloseFont(font64);
    TTF_CloseFont(font48);
    TTF_CloseFont(font32);
    TTF_CloseFont(font24);
    TTF_CloseFont(font16);

    delete background;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDLNet_Quit();
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char **argv) {
    GameManager manager;
    return manager.run();
}
