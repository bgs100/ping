#include "ErrorScreen.h"
#include "GameManager.h"

Texture *ErrorScreen::errorText = NULL;
Texture *ErrorScreen::continueText = NULL;

ErrorScreen::ErrorScreen(GameManager *m, const char *msg) : GameState(m) {
    if (errorText == NULL) {
        errorText = Texture::fromText(m->renderer, m->font48, "Error", 0xff, 0xff, 0xff);
        continueText = Texture::fromText(m->renderer, m->font24, "Press any key to continue", 0xff, 0xff, 0xff);
    }
    
    text = Texture::fromText(m->renderer, m->font32, msg, 0xff, 0xff, 0xff);
}

ErrorScreen::~ErrorScreen() {
    delete text;
}

void ErrorScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN)
        m->revertState();
}

void ErrorScreen::render() {
    errorText->render(m->renderer, (m->WIDTH - errorText->w) / 2, 200);
    text->render(m->renderer, (m->WIDTH - text->w) / 2, 350);
    continueText->render(m->renderer, (m->WIDTH - continueText->w) / 2, 500);
}
