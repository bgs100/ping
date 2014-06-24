#include <SDL2/SDL_ttf.h>
#include "DifficultyMenu.h"
#include "GameManager.h"
#include "KeyboardInput.h"
#include "AIInput.h"

const char *DifficultyMenu::labels[] = { "Easy", "Medium", "Hard", "Nope", "Insanity" };

DifficultyMenu::DifficultyMenu(GameManager *m) : GameState(m) {}

bool DifficultyMenu::init() {
    int height = END_BUTTON*TTF_FontHeight(m->font48) + (END_BUTTON-1)*30;
    buttonMenu.init(m->renderer, m->font48, labels, END_BUTTON, m->WIDTH/2, (m->HEIGHT-height)/2, 30, true);
    return true;
}

void DifficultyMenu::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        buttonMenu.selectButton(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int selected = buttonMenu.getSelected();
        if (selected != -1) {
            Game *game = new Game(m);
            m->pushState(game);
            game->init(new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), new AIInput((AIInput::Difficulty)selected));
        }
    } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        m->revertState();
    }
}

void DifficultyMenu::render() {
    buttonMenu.render(m->renderer);
}
