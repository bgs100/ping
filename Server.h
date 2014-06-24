// -*- c++ -*-
#ifndef PING_SERVER_H
#define PING_SERVER_H

#define PING_SERVER_BUILD
#include "StateListener.h"
#include "SharedState.h"

namespace Client {
    enum ClientCode { MOVE = 1 };
}

class Server: public StateListener {
public:
    enum ServerCode { STATE = 1, DISCONNECT, FULL };

    Server();
    void onBounce();
    void onHit();
    int run();

private:
    IPaddress serverIP;
    SDLNet_SocketSet socketSet;
    TCPsocket server;
    TCPsocket clients[2];
    bool bounce, hit;

    SharedState state;

    bool init();
    void handleActivity();
    void update();
};

#endif
