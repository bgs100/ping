#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <SDL2/SDL.h>

// Ugh!
class GameManager;

class GameState {
 public:
    GameState(GameManager *m) {
        this->m = m;
    }
    virtual bool init() { return true; }
    virtual void render() {}
    virtual void handleEvent(SDL_Event &event) {}
    virtual void handleInput() {}
    virtual void update() {}

 protected:
    GameManager *m;
};
    
#endif
