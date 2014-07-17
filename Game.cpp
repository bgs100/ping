#include <sstream>
#include "GameManager.h"
#include "Game.h"
#include "GameState.h"
#include "ErrorScreen.h"
#include "Server.h"
#include "Texture.h"
#include "utility.h"

Texture Game::whiteTexture;

// classic and demo are false by default (see Game.h).
Game::Game(GameManager *m, std::vector<PaddleInput *> inputs, int wallsPerPlayer, bool classic, bool demo)
    : GameState(m), state(this), inputs(inputs), server(NULL), networked(false), classic(classic), demo(demo) {
    setupStatic();

    if (classic)
        state.resetClassic();
    else
        state.reset(inputs.size(), wallsPerPlayer);

    setupTextures();
}

Game::Game(GameManager *m, PaddleInput *input, const char *host)
    : GameState(m), inputs{input}, server(NULL), networked(true), demo(false) {
    setupStatic();

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

    char op = server->getByte();
    if (server->error) {
        errorScreen("Server disconnected.");
        return;
    } else if (op == Server::FULL) {
        errorScreen("Server is full.");
        return;
    } else if (op != Server::INIT) {
        errorScreen("Unknown response.");
        return;
    }

    playerNum = server->getByte();
    int numPlayers = server->getByte();
    int wallsPerPlayer = server->getByte();

    if (numPlayers == 2 && wallsPerPlayer == 2)
        classic = server->getByte();

    if (classic)
        state.resetClassic();
    else
        state.reset(numPlayers, wallsPerPlayer);

    setupTextures();

    for (auto &player : state.players) {
        player.x = server->getDouble();
        player.y = server->getDouble();
    }

    if (server->error)
        errorScreen("Server disconnected.");
}

Game::~Game() {
    for (PaddleInput *input : inputs)
        delete input;

    delete server;
}

void Game::setupTextures() {
    SDL_Texture *target = SDL_CreateTexture(m->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m->WIDTH, m->HEIGHT);
    SDL_SetRenderTarget(m->renderer, target);

    double distToCenter = state.centerY;
    if (state.boundaries.size() % 2 == 0)
        distToCenter = m->HEIGHT / (2 * cos(pi / state.boundaries.size()));

    SDL_SetRenderDrawColor(m->renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(m->renderer);
    SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);

    double w = 32 * state.scale, h = 12 * state.scale, total = w + 14 * state.scale;
    if (classic) {
        for (int b = 0; b <= m->HEIGHT / total; b++)
            whiteTexture.render(m->renderer, (m->WIDTH-h)/2, (int)round(b * total), (int)round(h), (int)round(w));
    } else {
        whiteTexture.setColorMod(0x88, 0x88, 0x88);
        for (unsigned int i = 0; i < state.boundaries.size(); i++) {
            Vector2 start = state.boundaries[i];
            Vector2 end = state.boundaries[(i + 1) % state.boundaries.size()];
            double angle = atan2((end-start).y, (end-start).x);
            angle += pi/2 - pi / state.boundaries.size();
            double x = start.x + w/2 * cos(angle) - w/2;
            double y = start.y + w/2 * sin(angle) - h/2;
            for (int b = 0; b <= (int)(distToCenter / total); b++) {
                whiteTexture.render(m->renderer, (int)round(x), (int)round(y), (int)round(w), (int)round(h), angle * 180/pi);
                x += total * cos(angle);
                y += total * sin(angle);
            }
        }
        whiteTexture.setColorMod(0xff, 0xff, 0xff);
    }

    SDL_SetRenderTarget(m->renderer, NULL);
    background = Texture(target);

    Uint32 rmask, gmask, bmask, amask;

    // This is annoying, but seemingly necessary since an amask of 0
    // means no real transparency, and a non-zero amask appears to
    // preclude having default r/g/bmask values of 0.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_Surface *overlaySurf = SDL_CreateRGBSurface(0, m->WIDTH, m->HEIGHT, 32, rmask, gmask, bmask, amask);

    for (int x = 0; x < m->WIDTH; x++) {
        for (int y = 0; y < m->HEIGHT; y++) {
            bool interior = true;
            for (unsigned int i = 0; i < state.boundaries.size(); i++) {
                Vector2 start = state.boundaries[i];
                Vector2 end = state.boundaries[(i + 1) % state.boundaries.size()];
                if (((end.x-start.x)*(y-start.y) - (end.y-start.y)*(x-start.x)) < -500) {
                    interior = false;
                    break;
                }
            }

            if (!interior)
                ((Uint32 *)overlaySurf->pixels)[y*m->WIDTH+x] = amask;
        }
    }

    overlay = Texture::fromSurface(m->renderer, overlaySurf);
    SDL_FreeSurface(overlaySurf);
}

void Game::setupStatic() {
    if (whiteTexture.empty()) {
        SDL_Surface *surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));
        whiteTexture = Texture::fromSurface(m->renderer, surface);
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

void Game::update() {
    if (!networked) {
        std::vector<int> inputValues(state.players.size());
        for (unsigned int i = 0; i < state.players.size(); i++)
            inputValues[i] = inputs[i]->update(state, i);
        state.update(inputValues);
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

            std::vector<Entity *> entities = state.getEntities();
            int changedEntities = server->getByte();
            for (int i = 0; i < changedEntities; i++) {
                int entityNum = server->getByte();
                int numUpdates = server->getByte();
                for (int up = 0; up < numUpdates; up++) {
                    int field = server->getByte();
                    Uint64 val = server->getUint64();
                    if (field == EntityField::X)
                        entities[entityNum]->x = *((double *)&val);
                    else if (field == EntityField::Y)
                        entities[entityNum]->y = *((double *)&val);
                    else if (field == EntityField::SCORE)
                        state.scores[entityNum-1] = val;
                }
            }
        } else if (op == Server::DISCONNECT) {
            // TODO: Indicate that a player left.
            server->getByte();
        } else {
            errorScreen("Unknown directive from server.");
            return;
        }
    }

    if (server->error) {
        errorScreen("Host disconnected.");
        return;
    }

    char buf[2];
    buf[0] = Client::MOVE;
    buf[1] = inputs[0]->update(state, 0);
    server->send(buf, 2);
}

void renderEntity(SDL_Renderer *renderer, Texture &texture, const Entity &entity, double lag) {
    double dX = entity.getDX(), dY = entity.getDY();
    texture.render(renderer, entity.x + lag * dX, entity.y + lag * dY, entity.w, entity.h, entity.orientation * 180/pi);
}

void Game::render(double lag) {
    background.render(m->renderer, 0, 0);

    char buf[21]; // Max number of characters for a 64-bit int in base 10.
    Texture score;

    if (classic) {
        score = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_48], itoa(state.scores[0], buf, 21));
        score.render(m->renderer, m->WIDTH/4 - score.w/2, 40);
        score = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_48], itoa(state.scores[1], buf, 21));
        score.render(m->renderer, m->WIDTH*3/4 - score.w/2, 40);
    } else {
        int wallMult = state.boundaries.size() / state.scores.size();
        for (unsigned int i = 0; i < state.scores.size(); i++) {
            score = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_32], itoa(state.scores[i], buf, 21), 0xaa, 0xaa, 0xaa);
            Vector2 midpoint = (state.boundaries[(state.playerBoundaryOffset + i*wallMult) % state.boundaries.size()] +
                                state.boundaries[(state.playerBoundaryOffset + i*wallMult-1) % state.boundaries.size()]) / 2;
            double angle = pi/2 - (i*wallMult + state.playerBoundaryOffset) * 2*pi / state.boundaries.size();
            Vector2 center(midpoint.x + 70 * cos(angle), midpoint.y - 70 * sin(angle));
            double theta = pi/2 - angle;
            if (fmod(theta + 90, 2*pi) > pi)
                theta += pi;
            score.render(m->renderer, center.x - score.w/2.0, center.y - score.h/2.0, theta * 180/pi);
        }
    }

    for (auto p = state.players.begin(); p < state.players.end(); ++p) {
        renderEntity(m->renderer, whiteTexture, *p, lag);
        // Debugging points.
        SDL_SetRenderDrawColor(m->renderer, 0, 0, 0xff, 0xff);
        SDL_RenderDrawPoint(m->renderer, p->getVertices()[0].x,  p->getVertices()[0].y);
        SDL_SetRenderDrawColor(m->renderer, 0xff, 0, 0, 0xff);
        SDL_RenderDrawPoint(m->renderer, p->getVertices()[1].x,  p->getVertices()[1].y);
        SDL_SetRenderDrawColor(m->renderer, 0, 0xff, 0, 0);
        SDL_RenderDrawPoint(m->renderer, p->getVertices()[2].x,  p->getVertices()[2].y);
    }
    renderEntity(m->renderer, whiteTexture, state.ball, lag);

    SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);

    SDL_Point points[state.boundaries.size()+1];
    for (unsigned int i = 0; i < state.boundaries.size(); i++) {
        points[i].x = round(state.boundaries[i].x);
        points[i].y = round(state.boundaries[i].y);
    }

    points[state.boundaries.size()].x = round(state.boundaries[0].x);
    points[state.boundaries.size()].y = round(state.boundaries[0].y);

    SDL_RenderDrawLines(m->renderer, points, state.boundaries.size()+1);

    SDL_SetRenderDrawBlendMode(m->renderer, SDL_BLENDMODE_BLEND);
    overlay.render(m->renderer, 0, 0);
    SDL_SetRenderDrawBlendMode(m->renderer, SDL_BLENDMODE_NONE);
}
