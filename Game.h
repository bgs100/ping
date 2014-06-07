#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
    TTF_Font *font;
    bool running;
    Entity player, opponent, ball;
    int score1, score2;

    SDL_Texture *textureText(const char *str, Uint8 r, Uint8 g, Uint8 b);

    bool init();
    void handleEvents();
    void update();
    void render();
};
