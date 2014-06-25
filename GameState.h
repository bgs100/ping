// -*- c++ -*-
#ifndef PING_GAME_STATE_H
#define PING_GAME_STATE_H

#include <SDL2/SDL.h>

// Ugh!
class GameManager;

class GameState {
 public:
    bool valid;

    GameState(GameManager *m) : valid(true), m(m) {}
    virtual ~GameState() {}
    virtual void handleEvent(SDL_Event &event) {}
    virtual void update() {}
    virtual void render() {}
    virtual void render(double lag) {
        render();
    }

 protected:
    GameManager *m;
};
    
#endif
