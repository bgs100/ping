#include <iostream>
#include "TitleScreen.h"
#include "GameManager.h"

bool TitleScreen::init() {
    titleText = Texture::fromText(m->renderer, m->font64, "PiNG", 0xff, 0xff, 0xff);
    playText = Texture::fromText(m->renderer, m->font32, "Press any key to play", 0xff, 0xff, 0xff);
    return true;
}

void TitleScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN) {
        m->state = &m->game;
        delete titleText;
        delete playText;
    }
}

void TitleScreen::handleInput() {}

void TitleScreen::render() {
    SDL_SetRenderDrawColor(m->renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(m->renderer);

    titleText->render(m->renderer, 100, 50);
    playText->render(m->renderer, 100, 600);
}
