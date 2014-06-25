#include <string.h>
#include <ctype.h>
#include "MultiplayerMenu.h"
#include "GameManager.h"
#include "KeyboardInput.h"

Texture *MultiplayerMenu::prompt = NULL;

MultiplayerMenu::MultiplayerMenu(GameManager *m) : GameState(m), inputTexture(NULL) {
    inputText[0] = '\0';

    SDL_StartTextInput();

    if (prompt == NULL)
        prompt = Texture::fromText(m->renderer, m->font24, "Enter server address as domain:port (default 5556)", 0xff, 0xff, 0xff);
}

MultiplayerMenu::~MultiplayerMenu() {
    delete inputTexture;
}

void MultiplayerMenu::handleEvent(SDL_Event &event) {
    bool renderText = false;

    // TODO: Add a cursor, implement arrow key functionality.
    if (event.type == SDL_KEYDOWN) {
        int len = strlen(inputText);
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_BACKSPACE && len > 0) {
            inputText[len-1] = '\0';
            renderText = true;
        } else if (SDL_GetModState() & KMOD_CTRL && key == SDLK_c) {
            SDL_SetClipboardText(inputText);
        } else if (SDL_GetModState() & KMOD_CTRL && key == SDLK_v) {
            strncpy(inputText, SDL_GetClipboardText(), 260);
            inputText[259] = '\0';
            renderText = true;
        } else if (key == SDLK_RETURN) {
            // TODO: Parse for a custom port.
            m->pushState(new Game(m, new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), inputText));
        } else if (key == SDLK_ESCAPE) {
            m->revertState();
        }
    } else if (event.type == SDL_TEXTINPUT) {
        char *text = event.text.text;
        if (!(SDL_GetModState() & KMOD_CTRL && (tolower(text[0]) == 'c' || tolower(text[0]) == 'v'))) {
            strncat(inputText, text, 260-strlen(inputText));
            renderText = true;
        }
    }

    if (renderText) {
        delete inputTexture;
        inputTexture = Texture::fromText(m->renderer, m->font16, inputText, 0xff, 0xff, 0xff);
    }
}

void MultiplayerMenu::render() {
    prompt->render(m->renderer, (m->WIDTH - prompt->w)/2, 260);

    SDL_SetRenderDrawColor(m->renderer, 0xaa, 0xaa, 0xaa, 0xff);
    SDL_Rect rect = { 190, 310, m->WIDTH - 2*190, TTF_FontHeight(m->font16) };
    SDL_RenderDrawRect(m->renderer, &rect);

    if (inputTexture != NULL)
        inputTexture->render(m->renderer, 200, 310);
}
