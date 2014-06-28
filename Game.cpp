#include <sstream>
#include "GameManager.h"
#include "Game.h"
#include "GameState.h"
#include "ErrorScreen.h"
#include "Server.h"
#include "Texture.h"
#include "utility.h"

// demo is false by default (see Game.h).
Game::Game(GameManager *m, PaddleInput *p1input, PaddleInput *p2input, bool demo)
    : GameState(m), state(this), playerInput(p1input),
      opponentInput(p2input), networked(false), demo(demo) {
}

Game::Game(GameManager *m, PaddleInput *p1input, const char *host)
    : GameState(m), playerInput(p1input), server(NULL), networked(true), demo(false) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, host, 5556) != 0) {
        std::stringstream ss;
        ss << "Failed to resolve host: " << host;
        errorScreen(ss.str().c_str());
        return;
    }

    TCPsocket serverSock = SDLNet_TCP_Open(&ip);
    if (serverSock == NULL) {
        errorScreen("Failed to open connection.");
        return;
    }

    server = new Socket(serverSock);
    if (server->error) {
        errorScreen("Failed to allocate socket set.");
        return;
    }

    if (server->ready(10000) < 1) {
        errorScreen("Connection to server timed out.");
        return;
    }

    playerNum = server->getByte();
    if (server->error)
        errorScreen("Server disconnected.");
}

Game::~Game() {
    delete playerInput;
    if (networked)
        delete server;
    else
        delete opponentInput;
}

void Game::errorScreen(const char *msg) {
    m->pushState(new ErrorScreen(m, msg));
    valid = false;
}

void Game::onBounce() {
    if (!demo)
        Mix_PlayChannel(-1, m->bounceSound, 0);
}

void Game::onHit() {
    if (!demo)
        Mix_PlayChannel(-1, m->hitSound, 0);
}

void Game::update() {
    if (!networked) {
        state.update(playerInput->update(state, 0), opponentInput->update(state, 1));
        return;
    }

    while (server->ready() && !server->error) {
        char op = server->getByte();
        if (op == Server::STATE) {
            char sounds = server->getByte();
            if (sounds & 1)
                onBounce();
            if (sounds & 2)
                onHit();

            state.score1 = server->getUint64();
            state.player.x = server->getDouble();
            state.player.y = server->getDouble();
            state.player.dX = server->getDouble();
            state.player.dY = server->getDouble();

            state.score2 = server->getUint64();
            state.opponent.x = server->getDouble();
            state.opponent.y = server->getDouble();
            state.opponent.dX = server->getDouble();
            state.opponent.dY = server->getDouble();

            state.ball.x = server->getDouble();
            state.ball.y = server->getDouble();
            state.ball.dX = server->getDouble();
            state.ball.dY = server->getDouble();

            // This allows both players to play as the paddle on the
            // left, so that they can both comfortably use the same
            // controls (W and S).
            if (playerNum == 1) {
                state.ball.x = m->WIDTH - state.ball.x - state.ball.w;
                state.ball.dY *= -1;

                double tmp = state.player.y;
                state.player.y = state.opponent.y;
                state.opponent.y = tmp;

                tmp = state.player.dY;
                state.player.dY = state.opponent.dY;
                state.opponent.dY = tmp;

                int tmpScore = state.score1;
                state.score1 = state.score2;
                state.score2 = tmpScore;
            }

        }
    }

    if (server->error) {
        errorScreen("Host disconnected.");
        return;
    }

    char buf[2];
    buf[0] = Client::MOVE;
    buf[1] = playerInput->update(state, 0);
    server->send(buf, 2);
}

void Game::render(double lag) {
    m->background.render(m->renderer, 0, 0);

    SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
    state.player.render(m->renderer, lag);
    state.opponent.render(m->renderer, lag);
    state.ball.render(m->renderer, lag);

    char buf[21]; // Max number of characters for a 64-bit int in base 10.
    Texture tScore1 = Texture::fromText(m->renderer, m->font48, itoa(state.score1, buf, 21), 0xff, 0xff, 0xff);
    Texture tScore2 = Texture::fromText(m->renderer, m->font48, itoa(state.score2, buf, 21), 0xff, 0xff, 0xff);
    tScore1.render(m->renderer, m->WIDTH/4 - tScore1.w/2, 40);
    tScore2.render(m->renderer, m->WIDTH*3/4 - tScore2.w/2, 40);
}
