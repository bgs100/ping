#include <SDL2/SDL_net.h>
#include <iostream>

int error(const char *msg) {
    std::cout << msg << ": " << SDLNet_GetError() << std::endl;
    return 1;
}

int main(int argc, char **argv) {
    IPaddress serverIP;
    TCPsocket server;
    TCPsocket clients[2] = {0};
    char buffer[256];

    if (SDLNet_Init() != 0)
        return error("SDLNet_Init()");

    SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(3);
    if (socketSet == NULL)
        return error("SDLNet_AllocSocketSet");

    if (SDLNet_ResolveHost(&serverIP, NULL, 9999) != 0)
        return error("SDLNet_ResolveHost");

    server = SDLNet_TCP_Open(&serverIP);
    if (server == NULL)
        return error("SDLNet_TCP_Open");

    SDLNet_TCP_AddSocket(socketSet, server);
    while (1) {
        int activeSockets = SDLNet_CheckSockets(socketSet, 0);
        if (!activeSockets)
            continue;

        if (SDLNet_SocketReady(server)) {
            if (clients[0] == NULL || clients[1] == NULL) {
                int n = clients[1] == NULL;
                clients[n] = SDLNet_TCP_Accept(server);
                SDLNet_TCP_AddSocket(socketSet, clients[n]);
                SDLNet_TCP_Send(clients[n], "Hello!\n", 8);
            } else {
                TCPsocket tmp = SDLNet_TCP_Accept(server);
                SDLNet_TCP_Send(tmp, "Sorry, full.\n", 14);
                SDLNet_TCP_Close(tmp);
            }
        }

        for (int i = 0; i < 2; i++) {
            while (clients[i] != NULL && SDLNet_SocketReady(clients[i])) {
                int recvLen = SDLNet_TCP_Recv(clients[i], buffer, 256);

                if (recvLen <= 0) {
                    std::cout << "Player " << i + 1 << " disconnected." << std::endl;
                    SDLNet_TCP_DelSocket(socketSet, clients[i]);
                    SDLNet_TCP_Close(clients[i]);
                    clients[i] = NULL;
                } else {
                    SDLNet_TCP_Send(clients[1-i], buffer, recvLen);
                }
            }
        }
    }

    return 0;
}
