// -*- c++ -*-
#ifndef PING_MULTIPLAYER_MENU
#define PING_MULTIPLAYER_MENU

#include "GameState.h"
#include "Texture.h"
#include "Textbox.h"

class MultiplayerMenu: public GameState {
public:
    MultiplayerMenu(GameManager *m);
    
    void handleEvent(SDL_Event &event);
    void render();
    void cleanup();

private:
    static Texture prompt;
    Textbox hostInput;
};

#endif
