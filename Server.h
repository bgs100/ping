// -*- c++ -*-
#ifndef PING_SERVER_H
#define PING_SERVER_H

#include <vector>
#include "StateListener.h"
#include "SharedState.h"

namespace Client {
    enum ClientCode { MOVE = 1 };
}

namespace EntityField {
    enum Field { X, Y, SCORE };
}

struct EntityUpdate {
    EntityField::Field field;
    Uint64 val;
};

class Server: public StateListener {
public:
    enum ServerCode { INIT = 1, STATE, DISCONNECT, FULL };

    Server(int numPlayers, int wallsPerPlayer);
    void onBounce();
    void onHit();
    int run();

private:
    IPaddress serverIP;
    SDLNet_SocketSet socketSet;
    TCPsocket server;
    std::vector<TCPsocket> clients;
    bool bounce, hit;

    SharedState state;

    bool init();
    void handleActivity();
    void update();
};

#endif
