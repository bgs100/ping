#include <string.h>
#include <ctype.h>
#include "MultiplayerMenu.h"
#include "GameManager.h"
#include "KeyboardInput.h"

Texture *MultiplayerMenu::prompt = NULL;

MultiplayerMenu::MultiplayerMenu(GameManager *m) : GameState(m), hostInput(m->font16, 190, 310, m->WIDTH-190*2) {
    if (prompt == NULL)
        prompt = Texture::fromText(m->renderer, m->font24, "Enter server address as domain:port (default 5556)", 0xff, 0xff, 0xff);
}

void MultiplayerMenu::handleEvent(SDL_Event &event) {
    if (hostInput.handleEvent(event)) {
        // TODO: Parse for a custom port.
        m->pushState(new Game(m, new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), hostInput.text.c_str()));
    }
}

void MultiplayerMenu::render() {
    prompt->render(m->renderer, (m->WIDTH - prompt->w)/2, 260);
    hostInput.render(m->renderer);
}
