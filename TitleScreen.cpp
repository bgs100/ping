#include "TitleScreen.h"
#include "GameManager.h"
#include "AIInput.h"
#include "DifficultyMenu.h"
#include "SetupState.h"
#include "MultiplayerMenu.h"
#include "DevConsole.h"

const char *TitleScreen::labels[] = { "Singleplayer", "Multiplayer (Local)", "Multiplayer (Networked)", "Tutorial", "Credits", "Quit" };

std::vector<PaddleInput *> TitleScreen::makeInputs(int n) {
    std::vector<PaddleInput *> inputs(n);
    for (int i = 0; i < n; i++)
        inputs[i] = new AIInput((AIInput::Difficulty)(rand() % AIInput::NUM_DIFFICULTY));

    return inputs;
}

TitleScreen::TitleScreen(GameManager *m)
    : GameState(m), titleText(Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_64], "PiNG")),
      buttonMenu(m->renderer, m->fonts[FONT_SQR][SIZE_32], labels, END_BUTTON, 100, 250, 21), numAI(rand() % 3 + 2),
      backgroundGame(m, makeInputs(numAI), numAI > 2 ? (rand() % 3 + 1) : 2, true) {
}

void TitleScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        buttonMenu.selectButton(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int selected = buttonMenu.getSelected();
        // TODO: Add in code for the other buttons.
        if (selected == SINGLEPLAYER)
            m->pushState(new DifficultyMenu(m));
        else if (selected == MULTIPLAYER_LOCAL) {
            //std::vector<PaddleInput *> inputs = { new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S),
            //                                      new KeyboardInput(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN) };
            //m->pushState(new Game(m, inputs, 2));
            m->pushState(new SetupState(m));
        } else if (selected == MULTIPLAYER_NET)
            m->pushState(new MultiplayerMenu(m));
        else if (selected == QUIT)
            m->running = false;
    } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKQUOTE)
        m->pushState(new DevConsole(m));
}

void TitleScreen::update() {
    backgroundGame.update();
}

void TitleScreen::render(double lag) {
    backgroundGame.render(lag);

    SDL_SetRenderDrawBlendMode(m->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(m->renderer, 0, 0, 0, 0xaa);
    SDL_RenderFillRect(m->renderer, NULL);

    SDL_SetRenderDrawBlendMode(m->renderer, SDL_BLENDMODE_NONE);

    titleText.render(m->renderer, 100, 50);
    buttonMenu.render(m->renderer);
}
