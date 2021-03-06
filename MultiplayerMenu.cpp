#include <string.h>
#include <ctype.h>
#include "MultiplayerMenu.h"
#include "GameManager.h"
#include "KeyboardInput.h"

Texture MultiplayerMenu::prompt;

MultiplayerMenu::MultiplayerMenu(GameManager *m) : GameState(m), hostInput(m->fonts[FONT_SQR][SIZE_16], 190, 310, m->WIDTH-190*2) {
    if (prompt.empty())
        prompt = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_24], "Enter server address as domain:port (default 5556)");
}

void MultiplayerMenu::handleEvent(SDL_Event &event) {
    if (hostInput.handleEvent(event)) {
        // TODO: Parse for a custom port.
        m->pushState(new Game(m, new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S), hostInput.text.c_str()));
    }
}

void MultiplayerMenu::render() {
    prompt.render(m->renderer, (m->WIDTH - prompt.w)/2, 260);
    hostInput.render(m->renderer);
}
