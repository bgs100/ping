// -*- c++ -*-
#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <SDL2/SDL_net.h>

class Socket {
public:
    bool error;

    Socket(TCPsocket sock);
    ~Socket();
    bool ready(int timeout=0);
    char getByte();
    Uint64 getUint64();
    double getDouble();
    void send(char *buffer, int size);

private:
    TCPsocket sock;
    SDLNet_SocketSet set;
};

#endif
