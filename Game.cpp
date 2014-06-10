#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "Game.h"

// Utility functions

bool error(const char *msg) {
    std::cerr << msg << std::endl;
    return false;
}

bool SDLerror(const char *msg) {
    std::cerr << msg << ": " << SDL_GetError() << std::endl;
    return false;
}

bool netError(const char *msg) {
    std::cerr << msg << ": " << SDLNet_GetError() << std::endl;
    return false;
}

double clamp(double set, double min, double max) {
    if (set > max) return max;
    if (set < min) return min;
    return set;
}

// This is dumb.
const char *itoa(int x) {
    std::stringstream ss;
    ss << x;
    return ss.str().c_str();
}

char *netReadLine(TCPsocket sock) {
    char *buffer = (char *)malloc(256);
    int i = 0;

    do {
        if (SDLNet_TCP_Recv(sock, buffer+i, 1) < 1) {
            netError("SDLNet_TCP_Recv");
            free(buffer);
            return NULL;
        }
        i++;
    } while (buffer[i-1] != '\n' && i < 256);

    SDLNet_TCP_Recv(sock, buffer+i, 1);
    return buffer;
}

bool checkCollision(Entity a, Entity b) {
    return (a.y + a.h >= b.y && a.y <= b.y + b.h &&
            a.x + a.w >= b.x && a.x <= b.x + b.w);
}

// Game methods

Game::Game(const char *host) : player(20, HEIGHT/2-30, 20, 80),
                               opponent(WIDTH-40, HEIGHT/2-40, 20, 80),
                               ball(WIDTH/2, HEIGHT/2-10, 20, 20),
                               host(host) {
    ball.dX = rand() % 2 * 4 - 2;
    ball.dY = rand() % 2 * 4 - 2;
    score1 = score2 = 0;
}

bool Game::init() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return SDLerror("SDL_Init");

    if (TTF_Init() != 0)
        return SDLerror("TTF_Init");

    if (host != NULL && SDLNet_Init() != 0) {
        netError("SDLNet_Init");
        host = NULL;
    }

    window = SDL_CreateWindow("PiNG", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
	return SDLerror("SDL_CreateWindow");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
        return SDLerror("SDL_CreateRenderer");

    SDL_Surface *tmp = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    if (tmp == NULL)
        return SDLerror("SDL_CreateRGBSurface");
    SDL_Rect line = { WIDTH/2 - 10, 0, 20, HEIGHT };
    SDL_FillRect(tmp, &line, SDL_MapRGB(tmp->format, 0xff, 0xff, 0xff));
    background = SDL_CreateTextureFromSurface(renderer, tmp);
    if (background == NULL)
        return SDLerror("SDL_CreateTextureFromSurface");
    SDL_FreeSurface(tmp);

    font = TTF_OpenFont("kenpixel.ttf", 48);
    if (font == NULL)
        return SDLerror("TTF_OpenFont");

    if (host != NULL) {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, host, 5556) != 0) {
            netError("SDLNet_ResolveHost");
            host = NULL;
        } else {
            server = SDLNet_TCP_Open(&ip);
            if (server == NULL) {
                netError("SDLNet_TCP_Open");
                host = NULL;
            } else {
                socketSet = SDLNet_AllocSocketSet(1);
                if (socketSet == NULL) {
                    netError("SDLNet_AllocSocketSet");
                    host = NULL;
                }
                SDLNet_TCP_AddSocket(socketSet, server);
            }
        }
    }

    return true;
}

bool Game::netWait() {
    if (SDLNet_CheckSockets(socketSet, 10000) < 1)
        return error("Connection to server timed out.");

    char buffer[2];
    int recvLen = SDLNet_TCP_Recv(server, buffer, 2);
    if (recvLen < 2)
        return error("Server disconnected.");

    std::cout << "This client is player " << buffer << "." << std::endl;

    char *msg = netReadLine(server);
    if (msg == NULL)
        return error("Server disconnected.");
    int seed = atoi(msg);
    srand(seed);
    ball.dX = rand() % 2 * 4 - 2;
    ball.dY = rand() % 2 * 4 - 2;

    if (buffer[0] == '2') {
        ball.dX *= -1;
        SDLNet_TCP_Send(server, "hi\n", 4);
        return true;
    }

    while (SDLNet_CheckSockets(socketSet, 10000) < 1)
        std::cerr << "Waiting for opponent to join..." << std::endl;

    char buffer2[4];
    recvLen = SDLNet_TCP_Recv(server, buffer2, 4);
    if (recvLen < 1)
        return error("Server disconnected.");
    else if (recvLen < 4)
        return error("Opponent disconnected.");
    else if (strcmp(buffer2, "hi\n") != 0)
        return error("Incorrect greeting.");

    return true;
}

void Game::handleInput() {
    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    Entity *paddles[2] = { &player, &opponent };
    SDL_Scancode up[2] = { SDL_SCANCODE_W, SDL_SCANCODE_UP };
    SDL_Scancode down[2] = { SDL_SCANCODE_S, SDL_SCANCODE_DOWN };
    for (int i = 0; i < 2; i++) {
        int min = -10, max = 10;
        double change = state[down[i]] - state[up[i]];
        if (abs(change + paddles[i]->dY) < abs(paddles[i]->dY)) {
            change *= 2;
            if (paddles[i]->dY > 0)
                min = 0;
            else
                max = 0;
        }
        paddles[i]->dY = clamp(paddles[i]->dY + change, min, max);
    }
}

void Game::update() {
    int s1 = 0, s2 = 0;
    while (host != NULL && SDLNet_CheckSockets(socketSet, 0)) {
        char *msg = netReadLine(server);
        if (msg == NULL) {
            error("Host disconnected.");
            host = NULL;
        } else {
            std::cout << msg;
            std::stringstream ss(msg);
            int bX;
            double bdX;
            ss >> bX >> ball.y >> bdX >> ball.dY >> opponent.y >> opponent.dY >> s1 >> s2;
            ball.x = WIDTH - bX - ball.w;
            ball.dX = -bdX;
            free(msg);
        }
    }

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
    bool which;
    if ((which = checkCollision(ball, player)) || checkCollision(ball, opponent)) {
        ball.dX *= -1.1;
        if (which) {
            ball.dY += player.dY / 2;
        } else {
            ball.dY += opponent.dY / 2;
        }
    }

    if (player.dY > 0)
        player.dY = clamp(player.dY - .1, 0, 10);
    else
        player.dY = clamp(player.dY + .1, -10, 0);

    if (opponent.dY > 0)
        opponent.dY = clamp(opponent.dY - .1, 0, 10);
    else
        opponent.dY = clamp(opponent.dY + .1, -10, 0);

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

    if (host != NULL) {
        if (s2 > score1)
            score1 = s2;
        if (s1 > score2)
            score2 = s1;

        std::stringstream ss;
        ss << ball.x << " " << ball.y << " " << ball.dX << " " << ball.dY << " "
           << player.y << " " << player.dY << " " << score1 << " " << score2 << "\n";
        std::string s = ss.str();
        SDLNet_TCP_Send(server, s.c_str(), s.length()+1);
    }
}

SDL_Texture *Game::textureText(const char *str, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Color color = { r, g, b, 0xff };
    SDL_Surface *text = TTF_RenderText_Solid(font, str, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text);
    SDL_FreeSurface(text);
    return texture;
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

    if (host != NULL && !netWait())
        host = NULL;

    while (running) {
        handleInput();
        update();
        render();
    }

    return 0;
}

int main(int argc, char **argv) {
    Game *game;
    if (argc > 1)
        game = new Game(argv[1]);
    else
        game = new Game();
    return game->run();
}
