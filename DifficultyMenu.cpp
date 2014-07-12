#include <SDL2/SDL_ttf.h>
#include "DifficultyMenu.h"
#include "GameManager.h"
#include "KeyboardInput.h"
#include "AIInput.h"

DifficultyMenu::DifficultyMenu(GameManager *m)
    : GameState(m), buttonMenu(m->renderer, m->fonts[FONT_SQR][SIZE_48], AIInput::DIFFICULTY_STRS, (int)AIInput::NUM_DIFFICULTY, m->WIDTH/2,
                               (m->HEIGHT-AIInput::NUM_DIFFICULTY*TTF_FontHeight(m->fonts[FONT_SQR][SIZE_48])-(AIInput::NUM_DIFFICULTY-1)*30)/2,
                               30, true) {
    
}

void DifficultyMenu::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION) {
        buttonMenu.selectButton(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int selected = buttonMenu.getSelected();
        if (selected != -1) {
            std::vector<PaddleInput *> inputs = { new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), new AIInput((AIInput::Difficulty)selected) };
            m->pushState(new Game(m, inputs, 2));
        }
    }
}

void DifficultyMenu::render() {
    buttonMenu.render(m->renderer);
}
