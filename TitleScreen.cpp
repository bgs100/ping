#include "TitleScreen.h"
#include "GameManager.h"
#include "KeyboardInput.h"
#include "DifficultyMenu.h"
#include "MultiplayerMenu.h"
#include "DevConsole.h"

const char *TitleScreen::labels[] = { "Singleplayer", "Multiplayer (Local)", "Multiplayer (Networked)", "Tutorial", "Credits", "Quit" };

TitleScreen::TitleScreen(GameManager *m)
    : GameState(m), titleText(Texture::fromText(m->renderer, m->font64, "PiNG", 0xff, 0xff, 0xff)),
      buttonMenu(m->renderer, m->font32, labels, END_BUTTON, 100, 250, 21) {
}

TitleScreen::~TitleScreen() {
    delete titleText;
}

void TitleScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        buttonMenu.selectButton(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int selected = buttonMenu.getSelected();
        // TODO: Add in code for the other buttons.
        if (selected == SINGLEPLAYER)
            m->pushState(new DifficultyMenu(m));
        else if (selected == MULTIPLAYER_LOCAL)
            m->pushState(new Game(m, new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), new KeyboardInput(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN)));
        else if (selected == MULTIPLAYER_NET)
            m->pushState(new MultiplayerMenu(m));
        else if (selected == QUIT)
            m->running = false;
    } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKQUOTE)
        m->pushState(new DevConsole(m));
}

void TitleScreen::render() {
    m->background->render(m->renderer, 0, 0);
    titleText->render(m->renderer, 100, 50);
    buttonMenu.render(m->renderer);
}
