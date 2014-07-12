#include "ErrorScreen.h"
#include "GameManager.h"

Texture ErrorScreen::errorText, ErrorScreen::continueText;

ErrorScreen::ErrorScreen(GameManager *m, const char *msg)
    : GameState(m), text(Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_32], msg)) {
    if (errorText.empty()) {
        errorText = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_48], "Error");
        continueText = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_24], "Press any key to continue");
    }
    
}

void ErrorScreen::handleEvent(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN)
        valid = false;
}

void ErrorScreen::render() {
    errorText.render(m->renderer, (m->WIDTH - errorText.w) / 2, 200);
    text.render(m->renderer, (m->WIDTH - text.w) / 2, 350);
    continueText.render(m->renderer, (m->WIDTH - continueText.w) / 2, 500);
}
