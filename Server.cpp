#include <SDL2/SDL_net.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include "Server.h"
#include "utility.h"

Server::Server(int numPlayers, int wallsPerPlayer)
    : clients(numPlayers, NULL), bounce(false), hit(false), state(numPlayers, wallsPerPlayer, this) {
    state.ball.v = 0;
}

bool Server::init() {
    srand(time(NULL));

    if (SDLNet_Init() != 0)
        return SDLerror("SDLNet_Init()");

    socketSet = SDLNet_AllocSocketSet(clients.size()+1);
    if (socketSet == NULL)
        return SDLerror("SDLNet_AllocSocketSet");

    if (SDLNet_ResolveHost(&serverIP, NULL, 5556) != 0)
        return SDLerror("SDLNet_ResolveHost");

    server = SDLNet_TCP_Open(&serverIP);
    if (server == NULL)
        return SDLerror("SDLNet_TCP_Open");

    SDLNet_TCP_AddSocket(socketSet, server);

    return true;
}

void Server::onBounce() { 
    bounce = true;
}

void Server::onHit() {
    hit = true;
}

void Server::handleActivity() {
    SDLNet_CheckSockets(socketSet, 0);

    while (SDLNet_SocketReady(server)) {
        int n = -1;
        for (unsigned int i = 0; n == -1 && i < clients.size(); i++) {
            if (clients[i] == NULL)
                n = i;
        }

        if (n != -1) {
            clients[n] = SDLNet_TCP_Accept(server);
            SDLNet_TCP_AddSocket(socketSet, clients[n]);

            int bufSize = 4 + 16 * state.players.size();
            char buf[bufSize];
            int pos = 0;
            buf[pos++] = Server::INIT;
            buf[pos++] = (char)n;
            buf[pos++] = (char)state.players.size();
            buf[pos++] = (char)state.boundaries.size() / state.players.size();
            for (const auto &player : state.players) {
                *((double *)&buf[pos]) = htond(player.x);
                pos += 8;
                *((double *)&buf[pos]) = htond(player.y);
                pos += 8;
            }
            SDLNet_TCP_Send(clients[n], buf, bufSize);

            bool full = true;
            for (auto c = clients.begin(); full && c < clients.end(); ++c) {
                if (*c == NULL)
                    full = false;
            }

            if (full)
                state.resetBall();
        } else {
            TCPsocket tmp = SDLNet_TCP_Accept(server);
            const char buf[] = { Server::FULL };
            SDLNet_TCP_Send(tmp, buf, 1);
            SDLNet_TCP_Close(tmp);
        }
    }

    std::vector<int> inputs(clients.size(), 0);

    for (unsigned int i = 0; i < clients.size(); i++) {
        while (clients[i] != NULL && SDLNet_SocketReady(clients[i])) {
            char buffer[2];
            int recvLen = SDLNet_TCP_Recv(clients[i], buffer, 2);

            if (recvLen < 2) {
                char buf[2] = { Server::DISCONNECT, (char)i };

                for (const auto &client : clients) {
                    if (client != NULL)
                        SDLNet_TCP_Send(client, buf, 2);
                }

                SDLNet_TCP_DelSocket(socketSet, clients[i]);
                SDLNet_TCP_Close(clients[i]);
                clients[i] = NULL;
                state.ball.v = 0;
            } else if (buffer[0] == Client::MOVE) {
                inputs[i] += buffer[1];
            }
        }
    }

    SharedState old(state);
    state.update(inputs);

    std::vector<Entity *> oldEntities = old.getEntities();
    std::vector<Entity *> currentEntities = state.getEntities();

    std::vector<EntityUpdate> updates[oldEntities.size()];

    for (unsigned int i = 0; i < oldEntities.size(); i++) {
        if (oldEntities[i]->x != currentEntities[i]->x)
            updates[i].push_back({ EntityField::X, *((Uint64 *)&currentEntities[i]->x) });
        if (oldEntities[i]->y != currentEntities[i]->y)
            updates[i].push_back({ EntityField::Y, *((Uint64 *)&currentEntities[i]->y) });
        // Would be nice to find a way to make this neater...
        if (i > 0 && old.scores[i-1] != state.scores[i-1])
            updates[i].push_back({ EntityField::SCORE, (Uint64)state.scores[i-1]});
    }

    int bufSize = 3;
    int changedEntities = 0;
    for (unsigned int i = 0; i < oldEntities.size(); i++) {
        if (!updates[i].empty()) {
            bufSize += 2;
            changedEntities++;
        }
        bufSize += updates[i].size() * 9;
    }

    char buf[bufSize];
    int pos = 0;
    buf[pos++] = Server::STATE;
    buf[pos++] = ((int)hit << 1) | (int)bounce; 
    buf[pos++] = changedEntities;

    for (unsigned int i = 0; i < oldEntities.size(); i++) {
        if (updates[i].empty())
            continue;
        buf[pos++] = i;
        buf[pos++] = updates[i].size();
        for (const auto &update : updates[i]) {
            buf[pos++] = update.field;
            *((Uint64 *)&buf[pos]) = hton64(update.val);
            pos += 8;
        }
    }

    for (unsigned int i = 0; i < clients.size(); i++) {
        if (clients[i] != NULL)
            SDLNet_TCP_Send(clients[i], buf, bufSize);
    }

    if (bounce)
        bounce = false;
    if (hit)
        hit = false;
}

int Server::run() {
    if (!init())
        return 1;

    Uint32 time, last = SDL_GetTicks();
    double lag = 0;
    const double MS_PER_UPDATE = 1000.0 / 60.0;
    while (true) {
        if (lag < MS_PER_UPDATE) {
            SDL_Delay(round(MS_PER_UPDATE - lag));
        }
        while (lag >= MS_PER_UPDATE) {
            handleActivity();
            lag -= MS_PER_UPDATE;
        }
        time = SDL_GetTicks();
        lag += time - last;
        last = time;
    }
    
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "usage: ./server [number of players] [walls per player (defaults to 1)]" << std::endl;
        return 1;
    }

    int wallsPerPlayer = 1;
    if (argc > 2)
        wallsPerPlayer = std::stoi(argv[2]);

    Server server(std::stoi(argv[1]), wallsPerPlayer);
    return server.run();
}
