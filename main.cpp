#include <iostream>
#include "Game.h"

bool error(const char *msg) {
    std::cout << msg << ": " << SDL_GetError() << std::endl;
    return false;
}

double clamp(double set, double min, double max) {
    if (set > max) return max;
    if (set < min) return min;
    return set;
}

Game::Game() : player(20, HEIGHT/2-30, 20, 80), opponent(WIDTH-40, HEIGHT/2-40, 20, 80),
               ball(WIDTH/2, HEIGHT/2-10, 20, 20) {
    ball.dX = 1;
    ball.dY = -1;
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return error("SDL_Init");

    window = SDL_CreateWindow("PiNG", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
	return error("SDL_CreateWindow");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
        return error("SDL_CreateRenderer");

    SDL_Surface *tmp = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    if (tmp == NULL)
        return error("SDL_CreateRGBSurface");
    SDL_Rect line = { WIDTH/2 - 10, 0, 20, HEIGHT };
    SDL_FillRect(tmp, &line, SDL_MapRGB(tmp->format, 0xff, 0xff, 0xff));
    background = SDL_CreateTextureFromSurface(renderer, tmp);
    if (background == NULL)
        return error("SDL_CreateTextureFromSurface");
    SDL_FreeSurface(tmp);

    return true;
}

void Game::handleEvents() {
    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        else if (event.type == SDL_KEYDOWN) {
            int sym = event.key.keysym.sym;
            Entity *thing = NULL;
            double change;
            int min = -10, max = 10;

            if (sym == SDLK_w || sym == SDLK_s)
                thing = &player;
            else if (sym == SDLK_UP || sym == SDLK_DOWN)
                thing = &opponent;
            else
                continue;

            if (sym == SDLK_w || sym == SDLK_UP)
                change = -1;
            else
                change = 1;
            // Braking should be faster than accelerating.
            if (abs(change + thing->dY) < abs(thing->dY)) {
                change *= 3;
                if (thing->dY > 0)
                    min = 0;
                else
                    max = 0;
            }
            thing->dY = clamp(thing->dY + change, min, max);
        }
    }
}

bool checkCollision(Entity a, Entity b) {
    return (a.y + a.h >= b.y && a.y <= b.y + b.h &&
            a.x + a.w >= b.x && a.x <= b.x + b.w);
}

void Game::update() {
    player.y += player.dY;
    if (player.y < 0 || player.y + player.h > HEIGHT) {
        player.y = clamp(player.y, 0, HEIGHT-player.h);
        player.dY = 0;
    }
    opponent.y += opponent.dY;
    if (opponent.y < 0 || opponent.y + opponent.h > HEIGHT) {
        opponent.y = clamp(opponent.y, 0, HEIGHT-opponent.h);
        opponent.dY = 0;
    }

    ball.x += ball.dX;
    ball.y = clamp(ball.y + ball.dY, 0, HEIGHT - ball.h);
    if (ball.y <= 0 || ball.y + ball.h >= HEIGHT)
        ball.dY *= -1;
    if (checkCollision(ball, player) || checkCollision(ball, opponent))
        ball.dX *= -1;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, background, NULL, NULL);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    player.draw(renderer);
    opponent.draw(renderer);
    ball.draw(renderer);

    SDL_RenderPresent(renderer);
}

int Game::run() {
    running = true;

    if (!init())
        return 1;

    while (running) {
        handleEvents();
        update();
        render();
    }

    return 0;
}

int main(int argc, char **argv) {
    Game game;
    return game.run();
}
