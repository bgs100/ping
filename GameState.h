// -*- c++ -*-
#ifndef PING_GAME_STATE_H
#define PING_GAME_STATE_H

#include <SDL2/SDL.h>

// Ugh!
class GameManager;

class GameState {
 public:
    GameState(GameManager *m) : m(m) {}
    virtual ~GameState() {}
    virtual bool init() { return true; }
    virtual void render() {}
    virtual void handleEvent(SDL_Event &event) {}
    virtual void update(int delta) {}

 protected:
    GameManager *m;
};
    
#endif
