// -*- c++ -*-
#ifndef PING_TITLE_SCREEN_H
#define PING_TITLE_SCREEN_H

#include <SDL2/SDL.h>
#include "GameState.h"
#include "Game.h"
#include "Texture.h"
#include "ButtonMenu.h"

class TitleScreen: public GameState {
public:
    TitleScreen(GameManager *m);

    void handleEvent(SDL_Event &event);
    void update();
    void render(double lag);

private:
    enum Button { LOCAL_GAME, CLASSIC_GAME, NETWORK_GAME, TUTORIAL, CREDITS, QUIT, END_BUTTON };
    static const char *labels[];

    Texture titleText;
    ButtonMenu buttonMenu;
    int numAI;
    Game backgroundGame;

    static std::vector<PaddleInput *> makeInputs(int n);
};

#endif
