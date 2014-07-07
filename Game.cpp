#include <sstream>
#include "GameManager.h"
#include "Game.h"
#include "GameState.h"
#include "ErrorScreen.h"
#include "Server.h"
#include "Texture.h"
#include "utility.h"

Texture Game::paddle, Game::ball;

// demo is false by default (see Game.h).
Game::Game(GameManager *m, PaddleInput *p1input, PaddleInput *p2input, bool demo)
    : GameState(m), state(3, this), playerInput(p1input),
      opponentInput(p2input), networked(false), demo(demo) {
    setupTextures();
}

Game::Game(GameManager *m, PaddleInput *p1input, const char *host)
    : GameState(m), playerInput(p1input), server(NULL), networked(true), demo(false) {
    setupTextures();

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

void Game::setupTextures() {
    if (paddle.empty() || ball.empty()) {
        SDL_Surface *surface = SDL_CreateRGBSurface(0, state.players[0].w, state.players[0].h, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));
        paddle = Texture::fromSurface(m->renderer, surface);
        SDL_FreeSurface(surface);

        surface = SDL_CreateRGBSurface(0, state.ball.w, state.ball.h, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));
        ball = Texture::fromSurface(m->renderer, surface);
        SDL_FreeSurface(surface);
    }
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

// TODO TODO TODO
#include <iostream>
#include "KeyboardInput.h"

void Game::update() {
    KeyboardInput kb1(SDL_SCANCODE_I, SDL_SCANCODE_K), kb2(SDL_SCANCODE_O, SDL_SCANCODE_L);
    if (!networked) {
        std::vector<int> inputs(state.players.size());
        inputs[0] = playerInput->update(state, 0);
        inputs[1] = opponentInput->update(state, 1);
        inputs[2] = kb1.update(state, 2);
        for (unsigned int i = 3; i < state.players.size(); i++)
            inputs[i] = kb2.update(state, 0);
        state.update(inputs);
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

            // TODO: Implement new netcode.
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

void renderEntity(SDL_Renderer *renderer, Texture &texture, const Entity &entity, double lag) {
    double dX = entity.getDX(), dY = entity.getDY();
    texture.render(renderer, entity.x + lag * dX, entity.y + lag * dY, entity.orientation * 180/pi);
}

void Game::render(double lag) {
    m->background.render(m->renderer, 0, 0);

    char buf[21]; // Max number of characters for a 64-bit int in base 10.
    Texture score;
    for (unsigned int i = 0; i < state.scores.size(); i++) {
        score = Texture::fromText(m->renderer, m->font32, itoa(state.scores[i], buf, 21), 0xaa, 0xaa, 0xaa);
        Vector2 midpoint = (state.boundaries[(state.playerBoundaryOffset+i)%state.boundaries.size()] + state.boundaries[(state.playerBoundaryOffset+i-1)%state.boundaries.size()]) / 2;
        double angle = pi/2 - (i + state.playerBoundaryOffset) * 2*pi / state.players.size();
        Vector2 center(midpoint.x + 70 * cos(angle), midpoint.y - 70 * sin(angle));
        double theta = pi/2 - angle;
        if (fmod(theta + 90, 2*pi) > pi)
            theta += pi;
        score.render(m->renderer, center.x - score.w/2.0, center.y - score.h/2.0, theta * 180/pi);
    }

    for (auto p = state.players.begin(); p < state.players.end(); ++p) {
        renderEntity(m->renderer, paddle, *p, lag);
        SDL_SetRenderDrawColor(m->renderer, 0, 0, 0xff, 0xff);
        SDL_RenderDrawPoint(m->renderer, p->getVertices()[0].x,  p->getVertices()[0].y);
        SDL_SetRenderDrawColor(m->renderer, 0xff, 0, 0, 0xff);
        SDL_RenderDrawPoint(m->renderer, p->getVertices()[1].x,  p->getVertices()[1].y);
        SDL_SetRenderDrawColor(m->renderer, 0, 0xff, 0, 0);
        SDL_RenderDrawPoint(m->renderer, p->getVertices()[2].x,  p->getVertices()[2].y);
    }
    renderEntity(m->renderer, ball, state.ball, lag);

    SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);

    SDL_Point points[state.boundaries.size()];
    for (unsigned int i = 0; i < state.boundaries.size(); i++) {
        points[i].x = round(state.boundaries[i].x);
        points[i].y = round(state.boundaries[i].y);
    }
    SDL_RenderDrawLines(m->renderer, points, state.boundaries.size());

    /* Two-player specific code.
    Texture tScore1 = Texture::fromText(m->renderer, m->font48, itoa(state.score1, buf, 21), 0xff, 0xff, 0xff);
    Texture tScore2 = Texture::fromText(m->renderer, m->font48, itoa(state.score2, buf, 21), 0xff, 0xff, 0xff);
    tScore1.render(m->renderer, m->WIDTH/4 - tScore1.w/2, 40);
    tScore2.render(m->renderer, m->WIDTH*3/4 - tScore2.w/2, 40);
    */
}
