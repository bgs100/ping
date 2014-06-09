#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "Entity.h"

class Game {
 public:
    static const int WIDTH = 1024;
    static const int HEIGHT = 768;
    Game(const char *host=NULL);
    int run();
 private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    TTF_Font *font;
    bool running;
    Entity player, opponent, ball;
    int score1, score2;
    const char *host;
    TCPsocket server;
    SDLNet_SocketSet socketSet;

    SDL_Texture *textureText(const char *str, Uint8 r, Uint8 g, Uint8 b);

    bool init();
    bool netWait();
    void handleInput();
    void update();
    void render();
};
