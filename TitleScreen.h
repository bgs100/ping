// -*- c++ -*-
#ifndef PING_TITLE_SCREEN_H
#define PING_TITLE_SCREEN_H

#include <SDL2/SDL.h>
#include "GameState.h"
#include "Texture.h"
#include "MultiplayerMenu.h"

enum Button { SINGLEPLAYER, MULTIPLAYER_LOCAL, MULTIPLAYER_NET, TUTORIAL, CREDITS, QUIT, END_BUTTON };

class TitleScreen: public GameState {
public:
    TitleScreen(GameManager *m);
    ~TitleScreen();
    bool init();
    void handleEvent(SDL_Event &event);
    void render();

private:
    Texture *titleText;
    Texture *selectedButtons[END_BUTTON];
    Texture *unselectedButtons[END_BUTTON];
    int selected;

    int getButton(int x, int y);
};

#endif
