#include "TitleScreen.h"
#include "GameManager.h"
#include "KeyboardInput.h"
#include "AIInput.h"

TitleScreen::TitleScreen(GameManager *m) : GameState(m), selected(-1) {}

TitleScreen::~TitleScreen() {
    delete titleText;
    for (int i = 0; i < END_BUTTON; i++) {
        delete selectedButtons[i];
        delete unselectedButtons[i];
    }
}

bool TitleScreen::init() {
    titleText = Texture::fromText(m->renderer, m->font64, "PiNG", 0xff, 0xff, 0xff);
    const char *buttonStrs[] = { "Singleplayer", "Multiplayer (Local)", "Multiplayer (Networked)", "Tutorial", "Credits", "Quit" };
    for (int i = 0; i < END_BUTTON; i++) {
        selectedButtons[i] = Texture::fromText(m->renderer, m->font32, buttonStrs[i], 0xff, 0xff, 0xff);
        unselectedButtons[i] = Texture::fromText(m->renderer, m->font32, buttonStrs[i], 0xaa, 0xaa, 0xaa);
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
        if (selected == SINGLEPLAYER) {
            Game *game = new Game(m);
            m->pushState(game);
            game->init(new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), new AIInput(game));
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
    titleText->render(m->renderer, 100, 50);

    for (int i = 0; i < END_BUTTON; i++) {
        if (i == selected)
            selectedButtons[i]->render(m->renderer, 100, 300 + i*70);
        else
            unselectedButtons[i]->render(m->renderer, 100, 300 + i*70);
    }
}
