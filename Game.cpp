#include <iostream>
#include <sstream>
#include <math.h>
#include "GameManager.h"
#include "Game.h"
#include "GameState.h"
#include "Texture.h"
#include "utility.h"

Game::Game(GameManager *m) : GameState(m),
                             player(20, m->HEIGHT/2-30, 20, 80),
                             opponent(m->WIDTH-40, m->HEIGHT/2-40, 20, 80),
                             ball(m->WIDTH/2, m->HEIGHT/2-10, 20, 20),
                             score1(0), score2(0) {
    ball.dX = rand() % 2 * 4 - 2;
    ball.dY = rand() % 2 * 4 - 2;
}

// host is NULL by default (see Game.h).
bool Game::init(const char *host) {
    if (host == NULL) {
        multiplayer = false;
        return true;
    }

    multiplayer = true;
    
    // TODO: Rather than just reverting back to the multiplayer
    // menu on failure, this should switch with an error screen.
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, host, 5556) != 0) {
        SDLerror("SDLNet_ResolveHost");
        m->revertState();
        return false;
    }

    server = SDLNet_TCP_Open(&ip);
    if (server == NULL) {
        SDLerror("SDLNet_TCP_Open");
        m->revertState();
        return false;
    }

    socketSet = SDLNet_AllocSocketSet(1);
    if (socketSet == NULL) {
        SDLerror("SDLNet_AllocSocketSet");
        m->revertState();
        return false;
    }

    SDLNet_TCP_AddSocket(socketSet, server);

    return netWait();
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

void Game::handleEvent(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        m->revertState();
}

void Game::handleInput(int delta) {
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
        paddles[i]->dY = clamp(paddles[i]->dY + change * delta / (1000.0 / 60.0), min, max);
    }
}

void Game::update(int delta) {
    handleInput(delta);

    // For convenience:
    double deltaD = delta / (1000.0 / 60.0);

    int s1 = 0, s2 = 0;
    while (multiplayer && SDLNet_CheckSockets(socketSet, 0)) {
        char *msg = netReadLine(server);
        if (msg == NULL) {
            error("Host disconnected.");
            // TODO: Rather than just reverting to the multiplayer
            // menu, this should display an error screen/dialog.
            m->revertState();
            return;
        } else {
            std::cout << msg;
            std::stringstream ss(msg);
            int bX;
            double bdX;
            ss >> bX >> ball.y >> bdX >> ball.dY >> opponent.y >> opponent.dY >> s1 >> s2;
            ball.x = m->WIDTH - bX - ball.w;
            ball.dX = -bdX;
            free(msg);
        }
    }

    player.y += player.dY * deltaD;
    if (player.y < 0 || player.y + player.h > m->HEIGHT) {
        player.y = clamp(player.y, 0, m->HEIGHT-player.h);
        player.dY = 0;
    }
    opponent.y += opponent.dY * deltaD;
    if (opponent.y < 0 || opponent.y + opponent.h > m->HEIGHT) {
        opponent.y = clamp(opponent.y, 0, m->HEIGHT-opponent.h);
        opponent.dY = 0;
    }

    ball.x += ball.dX * deltaD;
    // This has to be rounded; truncation causes undesirable behavior
    // with a dY between -.5 (inclusive) and -1 (exclusive), which can
    // cause the ball to get stuck on the top of the screen when it shouldn't.
    ball.y = round(ball.y + ball.dY * deltaD);
    if (ball.y < 0 || ball.y + ball.h > m->HEIGHT) {
        ball.dY *= -1;
        if (ball.y < 0)
            ball.y = 0;
        else
            ball.y = m->HEIGHT - ball.h;
        Mix_PlayChannel(-1, m->bounceSound, 0);
    }

    bool which;
    if ((which = checkCollision(ball, player)) || checkCollision(ball, opponent)) {
        ball.dX *= -1.1;
        if (which) {
            ball.dY += player.dY / 2;
        } else {
            ball.dY += opponent.dY / 2;
        }
        Mix_PlayChannel(-1, m->hitSound, 0);
    }

    if (player.dY > 0)
        player.dY = clamp(player.dY - .1*deltaD, 0, 10);
    else
        player.dY = clamp(player.dY + .1*deltaD, -10, 0);

    if (opponent.dY > 0)
        opponent.dY = clamp(opponent.dY - .1*deltaD, 0, 10);
    else
        opponent.dY = clamp(opponent.dY + .1*deltaD, -10, 0);

    if (ball.x + ball.w < 0 || ball.x > m->WIDTH) {
        if (ball.x + ball.w < 0) {
            score2++;
            ball.dX = 2;
        } else {
            score1++;
            ball.dX = -2;
        }
        ball.dY = rand() % 2 * 4 - 2;
        ball.x = m->WIDTH / 2 - ball.w / 2;
        ball.y = m->HEIGHT / 2 - ball.h / 2;
    }

    if (multiplayer) {
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

void Game::render() {
    SDL_SetRenderDrawColor(m->renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(m->renderer);

    m->background->render(m->renderer, 0, 0);

    SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
    player.draw(m->renderer);
    opponent.draw(m->renderer);
    ball.draw(m->renderer);

    Texture *tScore1 = Texture::fromText(m->renderer, m->font48, itoa(score1), 0xff, 0xff, 0xff);
    Texture *tScore2 = Texture::fromText(m->renderer, m->font48, itoa(score2), 0xff, 0xff, 0xff);
    tScore1->render(m->renderer, m->WIDTH/4 - tScore1->w/2, 40);
    tScore2->render(m->renderer, m->WIDTH*3/4 - tScore2->w/2, 40);
    delete tScore1;
    delete tScore2;
}
