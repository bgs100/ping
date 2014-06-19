#include "TitleScreen.h"
#include "GameManager.h"
#include "KeyboardInput.h"
#include "DifficultyMenu.h"
#include "MultiplayerMenu.h"

const char *TitleScreen::labels[] = { "Singleplayer", "Multiplayer (Local)", "Multiplayer (Networked)", "Tutorial", "Credits", "Quit" };

TitleScreen::TitleScreen(GameManager *m) : GameState(m) {}

TitleScreen::~TitleScreen() {
    delete titleText;
}

bool TitleScreen::init() {
    titleText = Texture::fromText(m->renderer, m->font64, "PiNG", 0xff, 0xff, 0xff);
    buttonMenu.init(m->renderer, m->font32, labels, END_BUTTON, 100, 250, 21);
    return true;
}

void TitleScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        buttonMenu.selectButton(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int selected = buttonMenu.getSelected();
        // TODO: Add in code for the other buttons.
        if (selected == SINGLEPLAYER) {
            DifficultyMenu *menu = new DifficultyMenu(m);
            m->pushState(menu);
            menu->init();
        } else if (selected == MULTIPLAYER_LOCAL) {
            Game *game = new Game(m);
            m->pushState(game);
            game->init(new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), new KeyboardInput(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN));
        } else if (selected == MULTIPLAYER_NET) {
            MultiplayerMenu *menu = new MultiplayerMenu(m);
            m->pushState(menu);
            menu->init();
        } else if (selected == QUIT) {
            m->running = false;
        }
    }
}

void TitleScreen::render() {
    m->background->render(m->renderer, 0, 0);
    titleText->render(m->renderer, 100, 50);
    buttonMenu.render(m->renderer);
}
