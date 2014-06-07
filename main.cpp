#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
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
    ball.dX = rand() % 2 * 4 - 2;
    ball.dY = rand() % 2 * 4 - 2;
    score1 = score2 = 0;
}

bool Game::init() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return error("SDL_Init");

    if (TTF_Init() != 0)
        return error("TTF_Init");

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

    font = TTF_OpenFont("kenpixel.ttf", 48);
    if (font == NULL)
        return error("TTF_OpenFont");

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
    if (checkCollision(ball, player) || checkCollision(ball, opponent)) {
        ball.dX *= -1.2;
    }
    if (ball.x + ball.w < 0 || ball.x > WIDTH) {
        if (ball.x + ball.w < 0) {
            score2++;
            ball.dX = 2;
        } else {
            score1++;
            ball.dX = -2;
        }
        ball.dY = rand() % 2 * 4 - 2;
        ball.x = WIDTH / 2 - ball.w / 2;
        ball.y = HEIGHT / 2 - ball.h / 2;
    }
}

SDL_Texture *Game::textureText(const char *str, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Color color = { r, g, b, 0xff };
    SDL_Surface *text = TTF_RenderText_Solid(font, str, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text);
    SDL_FreeSurface(text);
    return texture;
}

// This is dumb.
const char *itoa(int x) {
    std::stringstream ss;
    ss << x;
    return ss.str().c_str();
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, background, NULL, NULL);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    player.draw(renderer);
    opponent.draw(renderer);
    ball.draw(renderer);

    // Displaying text is a bit more work than I anticipated. :|
    int w, h;
    SDL_Texture *tScore1 = textureText(itoa(score1), 0xff, 0xff, 0xff);
    SDL_QueryTexture(tScore1, NULL, NULL, &w, &h);
    SDL_Rect dst1 = { WIDTH/4 - w/2, 40, w, h };
    SDL_RenderCopy(renderer, tScore1, NULL, &dst1);

    SDL_Texture *tScore2 = textureText(itoa(score2), 0xff, 0xff, 0xff);
    SDL_QueryTexture(tScore2, NULL, NULL, &w, &h);
    SDL_Rect dst2 = { WIDTH*3/4 - w/2, 40, w, h };
    SDL_RenderCopy(renderer, tScore2, NULL, &dst2);

    SDL_DestroyTexture(tScore1);
    SDL_DestroyTexture(tScore2);

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
