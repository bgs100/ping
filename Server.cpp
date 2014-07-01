#include <SDL2/SDL_net.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include "Server.h"
#include "utility.h"

Server::Server() : bounce(false), hit(false), state(this) {
    clients[0] = NULL;
    clients[1] = NULL;
    state.ball.v = 0;
}

bool Server::init() {
    srand(time(NULL));

    if (SDLNet_Init() != 0)
        return SDLerror("SDLNet_Init()");

    socketSet = SDLNet_AllocSocketSet(3);
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
    //if (!activeSockets)
    //    return;

    while (SDLNet_SocketReady(server)) {
        if (clients[0] == NULL || clients[1] == NULL) {
            int n = clients[0] != NULL;
            clients[n] = SDLNet_TCP_Accept(server);
            SDLNet_TCP_AddSocket(socketSet, clients[n]);
            char buf[1];
            if (n == 0)
                buf[0] = 0;
            else
                buf[0] = 1;
            if (clients[0] != NULL && clients[1] != NULL)
                state.reset();
            SDLNet_TCP_Send(clients[n], buf, 1);
        } else {
            TCPsocket tmp = SDLNet_TCP_Accept(server);
            const char buf[] = { Server::FULL };
            SDLNet_TCP_Send(tmp, buf, 14);
            SDLNet_TCP_Close(tmp);
        }
    }

    int inputs[2] = { 0, 0 };
    for (int i = 0; i < 2; i++) {
        while (clients[i] != NULL && SDLNet_SocketReady(clients[i])) {
            char buffer[2];
            int recvLen = SDLNet_TCP_Recv(clients[i], buffer, 2);

            if (recvLen < 2) {
                char buf[1] = { Server::DISCONNECT };
                if (clients[1-i] != NULL)
                    SDLNet_TCP_Send(clients[1-i], buf, 1);
                SDLNet_TCP_DelSocket(socketSet, clients[i]);
                SDLNet_TCP_Close(clients[i]);
                clients[i] = NULL;
                state.ball.v = 0;
            } else if (buffer[0] == Client::MOVE) {
                inputs[i] += buffer[1];
            }
        }
    }

    state.update(inputs[0], inputs[1]);

    for (int i = 0; i < 2; i++) {
        if (clients[i] != NULL) {
            char buf[138];
            int pos = 0;
            buf[pos] = Server::STATE;

            buf[pos+=1] = ((int)hit << 1) | (int)bounce;

            *((Uint64 *)&buf[pos+=1]) = hton64(state.score1);
            *((double *)&buf[pos+=8]) = htond(state.player.x);
            *((double *)&buf[pos+=8]) = htond(state.player.y);
            *((double *)&buf[pos+=8]) = htond(state.player.theta);
            *((double *)&buf[pos+=8]) = htond(state.player.v);
            *((double *)&buf[pos+=8]) = htond(state.player.orientation);

            *((Uint64 *)&buf[pos+=8]) = hton64(state.score2);
            *((double *)&buf[pos+=8]) = htond(state.opponent.x);
            *((double *)&buf[pos+=8]) = htond(state.opponent.y);
            *((double *)&buf[pos+=8]) = htond(state.opponent.theta);
            *((double *)&buf[pos+=8]) = htond(state.opponent.v);
            *((double *)&buf[pos+=8]) = htond(state.opponent.orientation);
            
            *((double *)&buf[pos+=8]) = htond(state.ball.x);
            *((double *)&buf[pos+=8]) = htond(state.ball.y);
            *((double *)&buf[pos+=8]) = htond(state.ball.theta);
            *((double *)&buf[pos+=8]) = htond(state.ball.v);
            *((double *)&buf[pos+=8]) = htond(state.ball.orientation);
        
            SDLNet_TCP_Send(clients[i], buf, 138);
        }
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
    Server server;
    return server.run();
}
