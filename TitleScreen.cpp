#include <iostream>
#include "TitleScreen.h"
#include "GameManager.h"

bool TitleScreen::init() {
    titleText = m->textureText(m->font64, "PiNG", 0xff, 0xff, 0xff, &titleW, &titleH);
    playText = m->textureText(m->font32, "Press any key to play", 0xff, 0xff, 0xff, &playW, &playH);
    return true;
}

void TitleScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN)
        m->state = &m->game;
}

void TitleScreen::handleInput() {}

void TitleScreen::render() {
    SDL_SetRenderDrawColor(m->renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(m->renderer);

    SDL_Rect dst = { 100, 50, titleW, titleH };
    SDL_RenderCopy(m->renderer, titleText, NULL, &dst);

    SDL_Rect dst2 = { 100, 600, playW, playH };
    SDL_RenderCopy(m->renderer, playText, NULL, &dst2);
}
