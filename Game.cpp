#include <sstream>
#include "GameManager.h"
#include "Game.h"
#include "GameState.h"
#include "ErrorScreen.h"
#include "Server.h"
#include "Texture.h"
#include "utility.h"

bool errorScreen(GameManager *m, const char *msg) {
    m->swapState(new ErrorScreen(m, msg));
    return false;
}

bool errorWithScreen(GameManager *m, const char *msg) {
    errorScreen(m, msg);
    return error(msg);
}

Game::Game(GameManager *m) : GameState(m), state(this), server(NULL) {
}

Game::~Game() {
    delete server;
    delete playerInput;
    delete opponentInput;
}

bool Game::init(PaddleInput *p1input, PaddleInput *p2input) {
    playerInput = p1input;
    opponentInput = p2input;
    networked = false;
    return true;
}

bool Game::init(PaddleInput *p1input, const char *host) {
    playerInput = p1input;
    opponentInput = NULL;

    networked = true;
    
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, host, 5556) != 0) {
        SDLerror("SDLNet_ResolveHost");
        std::stringstream ss;
        ss << "Failed to resolve host: " << host;
        return errorScreen(m, ss.str().c_str());
    }

    TCPsocket serverSock = SDLNet_TCP_Open(&ip);
    if (serverSock == NULL) {
        SDLerror("SDLNet_TCP_Open");
        return errorScreen(m, "Failed to open connection.");
    }

    server = new Socket(serverSock);
    if (server->error) {
        SDLerror("SDLNet_AllocSocketSet");
        return errorScreen(m, "Failed to allocate socket set.");
    }

    return netWait();
}

bool Game::netWait() {
    if (server->ready(10000) < 1)
        return errorWithScreen(m, "Connection to server timed out.");

    playerNum = server->getByte();
    if (server->error)
        return errorWithScreen(m, "Server disconnected.");

    return true;
}

void Game::handleEvent(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        m->revertState();
}

void Game::onBounce() {
    Mix_PlayChannel(-1, m->bounceSound, 0);
}

void Game::onHit() {
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
        errorWithScreen(m, "Host disconnected.");
        return;
    }

    char buf[2];
    buf[0] = Client::MOVE;
    buf[1] = playerInput->update(state, 0);
    server->send(buf, 2);
}

void Game::render(double lag) {
    m->background->render(m->renderer, 0, 0);

    SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
    state.player.render(m->renderer, lag);
    state.opponent.render(m->renderer, lag);
    state.ball.render(m->renderer, lag);

    char buf[21]; // Max number of characters for a 64-bit int in base 10.
    Texture *tScore1 = Texture::fromText(m->renderer, m->font48, itoa(state.score1, buf, 21), 0xff, 0xff, 0xff);
    Texture *tScore2 = Texture::fromText(m->renderer, m->font48, itoa(state.score2, buf, 21), 0xff, 0xff, 0xff);
    tScore1->render(m->renderer, m->WIDTH/4 - tScore1->w/2, 40);
    tScore2->render(m->renderer, m->WIDTH*3/4 - tScore2->w/2, 40);
    delete tScore1;
    delete tScore2;
}
