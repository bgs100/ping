#include "Socket.h"
#include "utility.h"

Socket::Socket(TCPsocket sock) : error(false), sock(sock) {
    set = SDLNet_AllocSocketSet(1);
    if (set == NULL)
        error = true;
    else
        SDLNet_TCP_AddSocket(set, sock);
}

Socket::~Socket() {
    SDLNet_TCP_Close(sock);
    if (set != NULL)
        SDLNet_FreeSocketSet(set);
}

// timeout is 0 by default (see Socket.h).
bool Socket::ready(int timeout) {
    return SDLNet_CheckSockets(set, timeout) > 0;
}

char Socket::getByte() {
    char buffer[1];
    if (SDLNet_TCP_Recv(sock, buffer, 1) < 1)
        error = true;
    return buffer[0];
}

Uint64 Socket::getUint64() {
    char buffer[8];
    if (SDLNet_TCP_Recv(sock, buffer, 8) < 8)
        error = true;
    return ntoh64(*(Uint64 *)buffer);
}

double Socket::getDouble() {
    char buffer[8];
    if (SDLNet_TCP_Recv(sock, buffer, 8) < 8)
        error = true;
    return ntohd(*(double *)buffer);
}

void Socket::send(char *buffer, int size) {
    if (SDLNet_TCP_Send(sock, buffer, size) < size)
        error = true;
}
