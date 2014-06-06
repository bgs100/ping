#include <SDL2/SDL.h>
#include "Entity.h"

class Game {
 public:
    static const int WIDTH = 1024;
    static const int HEIGHT = 768;
    Game();
    int run();
 private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    bool running;
    Entity player, opponent, ball;
    bool init();
    void handleEvents();
    void update();
    void render();
};
