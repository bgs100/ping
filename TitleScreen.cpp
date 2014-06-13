#include "TitleScreen.h"
#include "GameManager.h"

TitleScreen::TitleScreen(GameManager *m) : GameState(m), multiplayerMenu(m), selected(-1) {}

bool TitleScreen::init() {
    titleText = Texture::fromText(m->renderer, m->font64, "PiNG", 0xff, 0xff, 0xff);
    const char *buttonStrs[] = { "Multiplayer (Local)", "Multiplayer (Networked)", "Tutorial", "Credits", "Quit" };
    for (int i = 0; i < END_BUTTON; i++) {
        selectedButtons[i] = Texture::fromText(m->renderer, m->font32, buttonStrs[i], 0xff, 0xff, 0xff);
        unselectedButtons[i] = Texture::fromText(m->renderer, m->font32, buttonStrs[i], 0xcc, 0xcc, 0xcc);
    }
    return true;
}

int TitleScreen::getButton(int x, int y) {
    static int fontHeight = TTF_FontHeight(m->font32);
    if (x >= 100 && y >= 300 && y <= 300 + END_BUTTON*70 && (y-300) % 70 <= fontHeight) {
        int numButton = (y - 300) / 70;
        if ((x - 100) <= selectedButtons[numButton]->w)
            return numButton;
    }
    return -1;
}

void TitleScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        int button = getButton(event.motion.x, event.motion.y);
        selected = button;
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        // TODO: Add in code for the other buttons.
        if (selected == MULTIPLAYER_LOCAL) {
            m->state = &m->game;
            cleanup();
            m->game.init();
        } else if (selected == MULTIPLAYER_NET) {
            multiplayerMenu.init();
            m->state = &multiplayerMenu;
            cleanup();
        } else if (selected == QUIT) {
            m->running = false;
        }
    }
}

void TitleScreen::handleInput() {}

void TitleScreen::render() {
    SDL_SetRenderDrawColor(m->renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(m->renderer);

    titleText->render(m->renderer, 100, 50);

    for (int i = 0; i < END_BUTTON; i++) {
        if (i == selected)
            selectedButtons[i]->render(m->renderer, 100, 300 + i*70);
        else
            unselectedButtons[i]->render(m->renderer, 100, 300 + i*70);
    }
}

void TitleScreen::cleanup() {
    for (int i = 0; i < END_BUTTON; i++) {
        delete selectedButtons[i];
        delete unselectedButtons[i];
    }    
}
