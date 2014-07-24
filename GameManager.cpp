#include <stdlib.h>
#include <time.h>
#include "GameManager.h"
#include "utility.h"

const int GameManager::fontSizes[] = { 8, 12, 16, 24, 32, 48, 64 };

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
    
    // Enable anti-aliasing.
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("PiNG", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
	return SDLerror("SDL_CreateWindow");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
        return SDLerror("SDL_CreateRenderer");

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    for (int i = 0; i < SIZE_END; i++) {
        fonts[FONT_RND][i] = TTF_OpenFont("kenpixel.ttf", fontSizes[i]);
        if (fonts[FONT_RND][i] == NULL)
            return SDLerror("TTF_OpenFont");
    }

    for (int i = 0; i < SIZE_END; i++) {
        fonts[FONT_SQR][i] = TTF_OpenFont("kenpixel-square-mod.ttf", fontSizes[i]);
        if (fonts[FONT_SQR][i] == NULL)
            return SDLerror("TTF_OpenFont");
    }

    bounceSound = Mix_LoadWAV("boop.wav");
    hitSound = Mix_LoadWAV("hit.wav");
    if (bounceSound == NULL || hitSound == NULL)
        return SDLerror("Mix_LoadWAV");

    Mix_AllocateChannels(16);

    pushState(new TitleScreen(this));

    return true;
}

GameState *GameManager::getState() {
    while (!stateStack.back()->valid)
        revertState();
    return stateStack.back();
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

void GameManager::handleEvents() {
    static SDL_Event event;

    // This is something of a hack, to prevent a new state that called
    // SDL_StartTextInput() from receiving a new SDL_TEXTINPUT event
    // with the same key press which, as an SDL_KEYDOWN event, caused
    // the new state to be created in the first place; this can result
    // in, for example, an unwanted initial character in a textbox
    // (this happens with DevConsole, which is created by TitleScreen
    // after the backtick key is pressed).
    GameState *state = getState();

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE && stateStack.size() > 1) {
            revertState();
            state = getState();
        } else
            state->handleEvent(event);
    }
}

void GameManager::render(double lag) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    getState()->render(lag);

    SDL_RenderPresent(renderer);
}

int GameManager::run() {
    running = true;

    if (!init())
        return 1;

    const double MS_PER_UPDATE = 1000.0 / 60.0;
    Uint32 last, time = SDL_GetTicks();
    double lag = 0;

    while (running) {
        handleEvents();
        while (lag >= MS_PER_UPDATE) {
            getState()->update();
            lag -= MS_PER_UPDATE;
        }
        render(lag / MS_PER_UPDATE);
        last = time;
        time = SDL_GetTicks();
        lag += time - last;
    }

    cleanup();

    return 0;
}

void GameManager::cleanup() {
    for (std::vector<GameState *>::reverse_iterator it = stateStack.rbegin(); it != stateStack.rend(); it++)
        delete *it;

    Mix_FreeChunk(hitSound);
    Mix_FreeChunk(bounceSound);

    for (int i = 0; i < FONT_END; i++) {
        for (int j = 0; j < SIZE_END; j++) {
            TTF_CloseFont(fonts[i][j]);
        }
    }

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
